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
    return true;
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
    selector, uint32_t queue_family_index)
{
    uint32_t score = 0;

    return score;
}

static inline bool gpu_selector_retrieve_queues(const gpu_queue_selector*
    selector, const gpu_queue_select_query* query, vk_queue* queues, uint32_t
    queue_family_index)
{
    return true;
}

gpu_queue_select_query_status gpu_selector_select(gpu_queue_selector* selector,
    const gpu_queue_select_query* query, vk_queue* queues)
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

        ssize_t current_score = gpu_selector_rate_queue_family(selector, i);
        if (current_score > best_score) {
            best_score = current_score;
            best_idx = i;
        }
    }

    if (best_idx == -1) {
        return status;
    }

    bool retrieval_status = gpu_selector_retrieve_queues(selector, query,
        queues, best_idx);
    if (!retrieval_status) {
        return GPU_SELECTOR_UNABLE_TO_RETRIEVE_QUEUE_ERROR;
    }

    return GPU_SELECTOR_OK;
}

void gpu_queue_selector_destroy(gpu_queue_selector* selector) {
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

