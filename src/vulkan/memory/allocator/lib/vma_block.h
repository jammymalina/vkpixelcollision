#ifndef VMA_BLOCK_H
#define VMA_BLOCK_H

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#include "./vma_types.h"

typedef struct vma_block_allocation_info {
    uint32_t size;
    uint32_t align;
    VkDeviceSize granularity;
    vma_allocation_type allocation_type;
} vma_block_allocation_info;

typedef struct vma_block_chunk {
    uint32_t allocation_id;
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
    VkDevice device;
    VkDeviceMemory device_memory;
    VkDeviceSize size;
    VkDeviceSize allocated;
    vma_byte* data;
} vma_block;


void vma_block_init_empty(vma_block* block, uint32_t memory_type_index,
    VkDeviceSize size, vma_memory_usage usage);
bool vma_block_init(vma_block* block);
void vma_block_destroy(vma_block* block);

static inline bool vma_block_is_host_visible(const vma_block* block) {
    return block->usage != VMA_MEMORY_USAGE_CPU_ONLY;
}

bool vma_block_allocate(vma_block* block, vma_allocation* allocation,
    const vma_block_allocation_info* block_alloc_info);
void vma_block_free_allocation(vma_block* block, vma_allocation* allocation);

void vma_block_print_json(const vma_block* block);

#endif
