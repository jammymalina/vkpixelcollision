#ifndef VMA_ALLOCATOR_H
#define VMA_ALLOCATOR_H

#include <stdint.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>

#include "./vma_allocation_object_allocator.h"
#include "./vma_block_vector.h"

#define VMA_ALLOCATOR_ERROR_MAX_LENGTH 256

enum vma_allocator_create_flag_bits {
    VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT = 2
};

typedef struct vma_allocator_create_info {
    uint32_t flags;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkDeviceSize preferred_large_heap_block_size;
} vma_allocator_create_info;

typedef struct vma_allocator {
    struct {
        bool ok;
        char error[VMA_ALLOCATOR_ERROR_MAX_LENGTH];
    } status;

    bool use_KHR_dedicated_allocation;
    VkDevice device;
    vma_allocation_object_allocator allocation_object_allocator;

    VkDeviceSize heap_size_limit[VK_MAX_MEMORY_HEAPS];

    VkPhysicalDeviceProperties physical_device_props;
    VkPhysicalDeviceMemoryProperties mem_props;

    vma_block_vector block_vectors[VK_MAX_MEMORY_TYPES];

    VkDeviceSize preferred_large_heap_block_size;
    VkPhysicalDevice physical_device;

    uint32_t current_frame_index;
    uint32_t gpu_defragmentation_memory_type_bits;

    uint32_t next_pool_id;
} vma_allocator;

vma_allocator vma_create_allocator(const vma_allocator_create_info *alloc_create_info);

#endif