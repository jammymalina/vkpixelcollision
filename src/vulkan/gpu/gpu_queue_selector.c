#include "./gpu_queue_selector.h"

#include "../../logger/logger.h"
#include "../../memory/memory.h"
#include "../functions/functions.h"

static gpu_queue_selector* dundercreek = NULL;

void gpu_queue_selector_init_empty(gpu_queue_selector* selector) {
    selector->gpu = NULL;
    selector->records = NULL;
    selector->records_size = 0;
}

static inline bool gpu_queue_selector_init_records(gpu_queue_selector* s) {
    for (size_t i = 0; i < s->records_size; ++i) {
        const VkQueueFamilyProperties* family_props =
            &s->gpu->queue_family_props[i];
        s->records[i].priorities = NULL;
        s->records[i].priorities = mem_alloc(sizeof(float) *
            family_props->queueCount);
        CHECK_ALLOC_BOOL(s->records[i].priorities, "Unable to allocate"
            " priorities in gpu queue selector record");
    }

    return true;
}

bool gpu_queue_selector_init(gpu_queue_selector* selector, const
    gpu_queue_selector_create_info* selector_info)
{
    selector->gpu = selector_info->gpu;
    ASSERT_LOG_ERROR(selector_info->gpu->queue_family_props_size > 0, "Unable"
        " to create gpu queue selector - invalid queue family count");
    selector->records = mem_alloc(sizeof(gpu_queue_family_record) *
        selector_info->gpu->queue_family_props_size);
    CHECK_ALLOC_BOOL(selector->records, "Unable to create gpu queue selector -"
        " records allocation issue");
    selector->records_size = selector_info->gpu->queue_family_props_size;
    bool status = gpu_queue_selector_init_records(selector);
    ASSERT_LOG_ERROR(status, "Unable to init gpu queue selector records");

    return true;
}

uint32_t gpu_selector_get_used_queue_family_count(const gpu_queue_selector*
    selector)
{
    uint32_t count = 0;
    for (size_t i = 0; i < selector->records_size; ++i) {
        if (selector->records[i].queues_used > 0) {
            ++count;
        }
    }

    return count;
}

void gpu_selector_get_device_queue_create_info(const gpu_queue_selector*
    selector, VkDeviceQueueCreateInfo* queues_infos)
{
    uint32_t queue_idx = 0;
    for (size_t i = 0; i < selector->records_size; ++i) {
        if (selector->records[i].queues_used == 0) {
            continue;
        }
        queues_infos[queue_idx].sType =
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queues_infos[queue_idx].sType = NULL;
        queues_infos[queue_idx].queueFamilyIndex = i;
        queues_infos[queue_idx].queueCount = selector->records[i].queues_used;
        queues_infos[queue_idx].pQueuePriorities =
            selector->records[i].priorities;
        ++queue_idx;
    }
}

static inline bool gpu_selector_is_present_supported(const gpu_queue_selector*
    selector, const gpu_queue_select_query* query, uint32_t queue_family_index)
{
    VkBool32 support_present = VK_FALSE;
    CHECK_VK_BOOL(vkGetPhysicalDeviceSurfaceSupportKHR(
        selector->gpu->physical_device, queue_family_index, query->surface,
        &support_present));
    return (bool) support_present;
}

static inline gpu_selector_is_queue_family_valid(const gpu_queue_selector*
    selector, const gpu_queue_select_query* query, uint32_t queue_family_index)
{
    const VkQueueFamilyProperties* family_props =
        &selector->gpu->queue_family_props[queue_family_index];

    bool status = (family_props->queueFlags & query->usage == query->usage) &&
        (!query->support_present || gpu_selector_is_present_supported(selector,
            query, queue_family_index));

    return status;
}

static inline uint32_t gpu_selector_rate_queue_family(const gpu_queue_selector*
    selector, const gpu_queue_select_query* query, uint32_t queue_family_index)
{
    const VkQueueFamilyProperties* family_props =
        &selector->gpu->queue_family_props[queue_family_index];
    uint32_t score = 0;
    // we try to select queue family with the least overlap
    uint32_t overlap_score = 32 - count_bits_uint32_t(family_props->queueFlags &
        ~query->usage);
    // we try to select queue family that was already used
    uint32_t queue_count_score =
        selector->records[queue_family_index].queues_used > 0 ? 10 : 0;

    return 100 * overlap_score + queue_count_score;
}

void gpu_selector_update_record_cache(gpu_queue_selector* selector, const
    gpu_queue_select_query* query, uint32_t family_idx)
{
    gpu_queue_family_record* r = &selector->records[family_idx];
    r->queues_used += query->queue_count;
    for (size_t i = r->next_available_queue; i < r->queues_used; ++i) {
        r->priorities[i] = query->priority;
        ++r->next_available_queue;
    }
}

gpu_queue_select_query_status gpu_selector_select(gpu_queue_selector* selector,
    const gpu_queue_select_query* query, gpu_queue_select_response* response)
{
    if (!selector->gpu) {
        return GPU_SELECTOR_NOT_INIT_ERROR;
    }
    gpu_queue_select_query_status status =
        GPU_SELECTOR_SUITABLE_QUEUE_NOT_FOUND_ERROR;
    ssize_t best_idx = -1;
    ssize_t best_score = -1;
    for (size_t i = 0; i < selector->records_size; ++i) {
        const VkQueueFamilyProperties* family_props =
            &selector->gpu->queue_family_props[i];
        // if gpu meets the criteria
        if (!gpu_selector_is_queue_family_valid(selector, query, i)) {
            continue;
        }
        // if there are enough queues left
        size_t remaining_queues = family_props->queueCount -
            selector->records[i].queues_used;
        if (remaining_queues < query->queue_count) {
            status = GPU_SELECTOR_NOT_ENOUGH_QUEUES_ERROR;
            continue;
        }

        ssize_t score = gpu_selector_rate_queue_family(selector, query, i);
        if (score > best_score) {
            best_score = score;
            best_idx = i;
        }
    }

    if (best_idx == -1) {
        return status;
    }

    gpu_selector_update_record_cache(selector, query, best_idx);

    // Create response
    response->queue_family = best_idx;
    response->queue_count = query->queue_count;
    response->priority = query->priority;

    return GPU_SELECTOR_OK;
}

bool gpu_selector_retrieve_queues(const gpu_queue_selector*
    selector, const gpu_queue_select_query* query, vk_queue* queues, uint32_t
    queue_family_index)
{
    return true;
}

static void gpu_queue_selector_record_destroy(gpu_queue_family_record* rec) {
    mem_free(rec->priorities);
}

void gpu_queue_selector_destroy(gpu_queue_selector* selector) {
    for (size_t i = 0; i < selector->records_size; ++i) {
        gpu_queue_selector_record_destroy(&selector->records[i]);
    }
    mem_free(selector->records);
    gpu_queue_selector_init_empty(selector);
}

void create_gpu_queue_selector(const gpu_queue_selector_create_info*
    selector_info)
{
    dundercreek = mem_alloc(sizeof(gpu_queue_selector));
    CHECK_ALLOC(dundercreek, "Unable to create gpu queue selector - mem alloc"
        " issue");
    bool status = gpu_queue_selector_init(dundercreek, selector_info);
    ASSERT_LOG_ERROR_EXIT(status, "Unable to create queue selector");
}

gpu_queue_selector* retrieve_gpu_queue_selector() {
    return dundercreek;
}

void destroy_gpu_queue_selector() {
    if (dundercreek == NULL) {
        return;
    }
    gpu_queue_selector_destroy(dundercreek);
    dundercreek = NULL;
}

