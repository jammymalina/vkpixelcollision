#ifndef VMA_ALLOCATION_H
#define VMA_ALLOCATION_H

#include <stdint.h>
#include <vulkan/vulkan.h>

#include "./vma_types.h"

typedef struct vma_allocation_create_info {
    uint32_t size;
    uint32_t align;
    uint32_t memory_type_bits;

    vma_memory_usage usage;
    vma_allocation_type type;
} vma_allocation_create_info;

typedef struct vma_allocation {
    vma_block* block;
    uint32_t id;
    VkDeviceMemory device_memory;
    VkDeviceSize offset;
    VkDeviceSize size;
    vma_byte* data;
} vma_allocation;

#endif
