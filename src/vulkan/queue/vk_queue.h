#ifndef VULKAN_QUEUE_H
#define VULKAN_QUEUE_H

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../gpu/gpu.h"

typedef struct vk_queue {
    VkQueue handle;
    uint32_t family_index;
    uint32_t queue_index;
    VkQueueFlags usage;
    VkExtent3D min_image_transfer_granularity;
} vk_queue;

void vk_queue_init_empty(vk_queue* q);
void vk_queue_init(vk_queue* q, const gpu_info* gpu, uint32_t family_index,
    uint32_t queue_index);

#endif
