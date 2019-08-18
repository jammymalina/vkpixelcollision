#include "./vk_queue.h"

void vk_queue_init_empty(vk_queue* q) {
    q->handle = VK_NULL_HANDLE;
    q->family_index = 0;
    q->usage = 0;
    q->min_image_transfer_granularity.width = 0;
    q->min_image_transfer_granularity.height = 0;
    q->min_image_transfer_granularity.depth = 0;
}
