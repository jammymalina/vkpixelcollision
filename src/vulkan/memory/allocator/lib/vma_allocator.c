#include "./vma_allocator.h"

#include <stddef.h>

#include "../../../functions/functions.h"

static VkDeviceSize vma_calc_preferred_block_size(const vma_allocator* allocator, uint32_t mem_type_index) {
    const uint32_t heap_index = vma_memory_type_to_heap_index(allocator, mem_type_index);
    const VkDeviceSize heap_size = allocator->mem_props.memoryHeaps[heap_index].size;
    const bool is_small_heap = heap_size <= VMA_SMALL_HEAP_MAX_SIZE;
    return is_small_heap ? (heap_size / 8) : allocator->preferred_large_heap_block_size;
}

static void vma_init_heap_size_limit(vma_allocator* allocator, const VkDeviceSize* heap_size_limit) {
    if (heap_size_limit != NULL) {
        const uint32_t memory_heap_count = vma_retrieve_memory_heap_count(allocator);
        for (uint32_t heap_index = 0; heap_index < memory_heap_count; ++heap_index) {
            const VkDeviceSize limit = heap_size_limit[heap_index];
            if (limit != VK_WHOLE_SIZE) {
                allocator->heap_size_limit[heap_index] = limit;
                if (limit < allocator->mem_props.memoryHeaps[heap_index].size) {
                    allocator->mem_props.memoryHeaps[heap_index].size = limit;
                }
            }
        }
    }
}

static void vma_init_block_vectors(vma_allocator* allocator) {
    const uint32_t memory_type_count = vma_retrieve_memory_type_count(allocator);
    for (uint32_t mem_type_index = 0; mem_type_index < memory_type_count; ++mem_type_index) {
        const VkDeviceSize preferred_block_size = vma_calc_preferred_block_size(allocator, mem_type_index);
    }
}

void vma_create_allocator(vma_allocator* allocator, const vma_allocator_create_info* alloc_create_info) {
    allocator->status.initialized = false;
    allocator->use_KHR_dedicated_allocation 
        = (alloc_create_info->flags & VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT) != 0;
    allocator->device = alloc_create_info->device;
    allocator->preferred_large_heap_block_size = 0;
    allocator->physical_device = alloc_create_info->physical_device;
    allocator->current_frame_index = 0;
    allocator->gpu_defragmentation_memory_type_bits = UINT32_MAX;
    allocator->next_pool_id = 0;
    allocator->status.error[0] = '\0';

    for (size_t i = 0; i < VK_MAX_MEMORY_HEAPS; ++i) {
        allocator->heap_size_limit[i] = VK_WHOLE_SIZE;
    }

    vkGetPhysicalDeviceProperties(allocator->physical_device, &allocator->physical_device_props);
    vkGetPhysicalDeviceMemoryProperties(allocator->physical_device, &allocator->mem_props);

    allocator->preferred_large_heap_block_size = alloc_create_info->preferred_large_heap_block_size != 0 ?
        alloc_create_info->preferred_large_heap_block_size : VMA_DEFAULT_LARGE_HEAP_BLOCK_SIZE;
    
    vma_init_heap_size_limit(allocator, alloc_create_info->heap_size_limit);
    vma_init_block_vectors(allocator);
    
    allocator->status.initialized = true;
}