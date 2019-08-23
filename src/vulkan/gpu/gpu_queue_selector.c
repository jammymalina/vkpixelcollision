#include "./gpu_queue_selector.h"

#include "../../logger/logger.h"
#include "../../memory/memory.h"
#include "../../math/math.h"
#include "../../string/string.h"
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
        s->records[i].queue_records = NULL;
        s->records[i].queue_records = mem_alloc(sizeof(gpu_queue_record) *
            family_props->queueCount);
        CHECK_ALLOC_BOOL(s->records[i].queue_records, "Unable to allocate"
            " queue records in gpu queue selector record");
        for (size_t j = 0; j < family_props->queueCount; ++j) {
            string_copy(s->records[i].queue_records[j].group_name,
                GPU_QUEUE_MAX_GROUP_NAME_LENGTH, "");
            s->records[i].queue_records[j].priority = 1.0;
        }
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

bool gpu_selector_get_device_queue_create_info(const gpu_queue_selector*
    selector, VkDeviceQueueCreateInfo* queues_info)
{
    uint32_t queue_idx = 0;
    for (size_t i = 0; i < selector->records_size; ++i) {
        if (selector->records[i].queues_used == 0) {
            continue;
        }

        float* priorities = mem_alloc(sizeof(float) *
            selector->records[i].queues_used);
        CHECK_ALLOC_BOOL(priorities, "Unable to allocate queue priorities");

        for (size_t j = 0; j < selector->records[i].queues_used; ++j) {
            priorities[j] = selector->records[i].queue_records[j].priority;
        }

        queues_info[queue_idx].sType =
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queues_info[queue_idx].sType = NULL;
        queues_info[queue_idx].queueFamilyIndex = i;
        queues_info[queue_idx].queueCount = selector->records[i].queues_used;
        queues_info[queue_idx].pQueuePriorities = priorities;
        ++queue_idx;
    }

    return true;
}

void gpu_selector_destroy_device_queue_create_info(VkDeviceQueueCreateInfo*
    queues_info, size_t queues_info_size)
{
    for (size_t i = 0; i < queues_info_size; ++i) {
        mem_free(queues_info[i].pQueuePriorities);
        queues_info[0].pQueuePriorities = NULL;
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

static void gpu_selector_update_record_cache(gpu_queue_selector* selector, const
    gpu_queue_select_query* query, uint32_t family_idx)
{
    gpu_queue_family_record* r = &selector->records[family_idx];
    r->queues_used += query->queue_count;
    for (size_t i = r->next_available_queue; i < r->queues_used; ++i) {
        r->queue_records[i].priority = query->priority;
        string_copy(r->queue_records[i].group_name,
            GPU_QUEUE_MAX_GROUP_NAME_LENGTH, query->queue_group_name);
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

static void gpu_selector_retrive_queue(const gpu_queue_selector* selector,
    vk_queue* queue, uint32_t family_index, uint32_t queue_index)
{
    vk_queue_init(queue, selector->gpu, family_index, queue_index);
}

size_t gpu_selector_retrieve_queue_group(const gpu_queue_selector* selector,
    const char group_name[GPU_QUEUE_MAX_GROUP_NAME_LENGTH], vk_queue* queues)
{
    if (!selector->gpu->device) {
        log_warning("Unable to retrieve queues - device is not initialized");
        return 0;
    }

    size_t queue_count = 0;
    vk_queue* queue_iter = queues;
    for (size_t i = 0; i < selector->records_size; ++i) {
        const gpu_queue_family_record* r = &selector->records[i];
        for (size_t j = 0; j < r->queues_used; ++j) {
            const gpu_queue_record* qr = r->queue_records[j].group_name;
            if (!string_equal(qr->group_name, group_name)) {
                continue;
            }
            ++queue_count;
            if (queue_iter) {
                gpu_selector_retrive_queue(selector, queue_iter, i, j);
                ++queue_iter;
            }
        }
    }

    return queue_count;
}

static void gpu_queue_selector_record_destroy(gpu_queue_family_record* rec) {
    mem_free(rec->queue_records);
    rec->queue_records = NULL;
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

