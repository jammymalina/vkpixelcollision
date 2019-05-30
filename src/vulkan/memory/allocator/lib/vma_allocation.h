#ifndef VMA_ALLOCATION_H
#define VMA_ALLOCATION_H

#include <stdint.h>
#include <vulkan/vulkan.h>

#include "./vma_types.h"

typedef struct vma_allocation_h {
    vma_block* block;
    uint32_t id;
    VkDeviceMemory device_memory;
    VkDeviceSize offset;
    VkDeviceSize size;
    vma_byte* data;
} vma_allocation_h;

#endif