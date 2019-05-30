#ifndef VMA_BLOCK_H
#define VMA_BLOCK_H

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#include "./vma_types.h"

typedef struct vma_block_chunk {
    uint32_t id;
    VkDeviceSize size;
    VkDeviceSize offset;
    struct vma_block_chunk* prev;
    struct vma_block_chunk* next;
    vma_allocation_type allocation_type;
} vma_block_chunk;

typedef struct vma_block {
    vma_block_chunk* head;
    uint32_t next_block_id;
    uint32_t memory_type_index;
    vma_memory_usage usage;
    VkDeviceMemory device_memory;
    VkDeviceSize size;
    VkDeviceSize allocated;
    vma_byte* data;
} vma_block;

#endif