#include "./vk_queue.h"

#include "../functions/functions.h"

void vk_queue_init_empty(vk_queue* q) {
    q->handle = VK_NULL_HANDLE;
    q->family_index = 0;
    q->usage = 0;
    q->min_image_transfer_granularity.width = 0;
    q->min_image_transfer_granularity.height = 0;
    q->min_image_transfer_granularity.depth = 0;
}

void vk_queue_init(vk_queue* q, const gpu_info* gpu, uint32_t family_index,
    uint32_t queue_index)
{
    vk_queue_init_empty(q);
    const VkQueueFamilyProperties* props =
        &gpu->queue_family_props[family_index];
    q->family_index = family_index;
    q->usage = props->queueFlags;
    q->min_image_transfer_granularity = props->minImageTransferGranularity;
    vkGetDeviceQueue(gpu->device, family_index, queue_index, &q->handle);
}
