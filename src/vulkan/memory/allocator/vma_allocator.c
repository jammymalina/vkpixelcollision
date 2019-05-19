#include "./vma_allocator.h"

#include <stdio.h>

vma_allocator vma_create_allocator(const vma_allocator_create_info *alloc_create_info) {
    vma_allocator allocator = {
        .status = {
            .ok = true
        },
        .use_KHR_dedicated_allocation 
            = alloc_create_info->flags & VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT != 0,
        .device = alloc_create_info->device,
        .preferred_large_heap_block_size = 0,
        .physical_device = alloc_create_info->physical_device,
        .current_frame_index = 0,
        .gpu_defragmentation_memory_type_bits = UINT32_MAX,
        .next_pool_id = 0
    };
    snprintf(allocator.status.error, VMA_ALLOCATOR_ERROR_MAX_LENGTH, "");

    return allocator;
}