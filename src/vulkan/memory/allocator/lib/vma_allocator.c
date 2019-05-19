#include "./vma_allocator.h"

#include <stddef.h>

#include "../../../functions/functions.h" 

vma_allocator vma_create_allocator(const vma_allocator_create_info *alloc_create_info) {
    vma_allocator allocator = {
        .status = {
            .initialized = false
        },
        .use_KHR_dedicated_allocation 
            = (alloc_create_info->flags & VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT) != 0,
        .device = alloc_create_info->device,
        .preferred_large_heap_block_size = 0,
        .physical_device = alloc_create_info->physical_device,
        .current_frame_index = 0,
        .gpu_defragmentation_memory_type_bits = UINT32_MAX,
        .next_pool_id = 0
    };
    allocator.status.error[0] = '\0';

    for (size_t i = 0; i < VK_MAX_MEMORY_HEAPS; ++i) {
        allocator.heap_size_limit[i] = VK_WHOLE_SIZE;
    }

    vkGetPhysicalDeviceProperties(allocator.physical_device, &allocator.physical_device_props);
    vkGetPhysicalDeviceMemoryProperties(allocator.physical_device, &allocator.mem_props);

    allocator.preferred_large_heap_block_size = alloc_create_info->preferred_large_heap_block_size != 0 ?
        alloc_create_info->preferred_large_heap_block_size : VMA_DEFAULT_LARGE_HEAP_BLOCK_SIZE;
    
    if (alloc_create_info->heap_size_limit != NULL) {
        uint32_t memory_heap_count = allocator.mem_props.memoryHeapCount;
        for (uint32_t heap_index = 0; heap_index < memory_heap_count; ++heap_index) {
            const VkDeviceSize limit = alloc_create_info->heap_size_limit[heap_index];
            if (limit != VK_WHOLE_SIZE) {
                allocator.heap_size_limit[heap_index] = limit;
                if (limit < allocator.mem_props.memoryHeaps[heap_index].size) {
                    allocator.mem_props.memoryHeaps[heap_index].size = limit;
                }
            }
        }
    }

    uint32_t memory_type_count = allocator.mem_props.memoryTypeCount;
    for (uint32_t mem_type_index = 0; mem_type_index < memory_type_count; ++mem_type_index) {
        const VkDeviceSize preferred_block_size = 0;
    }

    allocator.status.initialized = true;

    return allocator;
}