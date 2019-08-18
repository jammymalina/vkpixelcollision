#ifndef VULKAN_QUEUE_H
#define VULKAN_QUEUE_H

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

typedef struct vk_queue {
    VkQueue handle;
    uint32_t family_index;
    uint32_t queue_index;
    VkQueueFlags usage;
    VkExtent3D min_image_transfer_granularity;
} vk_queue;

void vk_queue_init_empty(vk_queue* q);

#endif
