#ifndef VMA_ALLOCATOR_H
#define VMA_ALLOCATOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#include "./vma_types.h"

#include "./vma_block.h"
#include "./vma_allocation.h"

#define VMA_MINIMUM_BLOCKS_SIZE 10
#define VMA_MINIMUM_GARBAGE_SIZE 10

typedef struct vma_allocator_create_info {
    VkDeviceSize desired_device_local_memory_MB;
    VkDeviceSize desired_host_visible_memory_MB;
    VkDeviceSize buffer_image_granularity;
    uint32_t number_of_frames;
    uint32_t min_blocks_size;
    uint32_t min_garbage_size;
} vma_allocator_create_info;

typedef struct vma_allocator {
    int garbage_index;
    int32_t device_local_memory_bytes;
    int32_t host_visible_memory_bytes;
    uint32_t number_of_frames;
    VkDeviceSize buffer_image_granularity;

    vma_block_vector blocks[VK_MAX_MEMORY_TYPES];
    vma_allocation_vector* garbage;
} vma_allocator;

bool vma_allocator_init(vma_allocator* allocator,
    const vma_allocator_create_info* allocator_info);
void vma_allocator_destroy(vma_allocator* allocator);

bool vma_allocator_allocate(vma_allocator* allocator, vma_allocation* alloc,
    const vma_allocation_create_info* alloc_info);
bool vma_allocator_free_allocation(vma_allocator* allocator,
    const vma_allocation* allocation);

void vma_allocator_empty_garbage(vma_allocator* allocator);

void vma_allocator_print_json(const vma_allocator* allocator);

#endif
