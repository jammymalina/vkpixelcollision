#ifndef VMA_ALLOCATOR_H
#define VMA_ALLOCATOR_H

#include <stdint.h>
#include <vulkan/vulkan.h>

#include "./vma_block.h"
#include "./vma_types.h"

typedef struct vma_allocator {
    int garbage_index;
    int32_t device_local_memory_bytes;
    int32_t host_visible_memory_bytes;
    uint32_t number_of_frames;
    VkDeviceSize buffer_image_granularity;
} vma_allocator;

#endif