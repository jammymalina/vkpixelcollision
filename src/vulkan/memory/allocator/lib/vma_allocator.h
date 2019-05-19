#ifndef VMA_ALLOCATOR_H
#define VMA_ALLOCATOR_H

#include <stdint.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>
#include <assert.h>

#include "./vma_allocation_object_allocator.h"
#include "./vma_block_vector.h"

#define VMA_ALLOCATOR_ERROR_MAX_LENGTH 256

#ifndef VMA_DEFAULT_LARGE_HEAP_BLOCK_SIZE
    // Default size of a block allocated as single VkDeviceMemory from a "large" heap.
    #define VMA_DEFAULT_LARGE_HEAP_BLOCK_SIZE (256ull * 1024 * 1024)
#endif

#ifndef VMA_SMALL_HEAP_MAX_SIZE
   // Maximum size of a memory heap in Vulkan to consider it "small".
   #define VMA_SMALL_HEAP_MAX_SIZE (1024ull * 1024 * 1024)
#endif


#ifndef VMA_DEFAULT_ALIGNMENT
    #define VMA_DEFAULT_ALIGNMENT (1)
#endif

enum vma_allocator_create_flag_bits {
    VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT = 2
};

typedef struct vma_allocator_create_info {
    uint32_t flags;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkDeviceSize preferred_large_heap_block_size;
    const VkDeviceSize* heap_size_limit;
} vma_allocator_create_info;

typedef struct vma_allocator {
    struct {
        bool initialized;
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

vma_allocator vma_create_allocator(const vma_allocator_create_info* alloc_create_info);

static inline VkDeviceSize vma_retrieve_buffer_image_granularity(const vma_allocator* allocator) {
    return allocator->physical_device_props.limits.bufferImageGranularity;
}

static inline uint32_t vma_retrieve_memory_heap_count(const vma_allocator* allocator) {
    return allocator->mem_props.memoryHeapCount;
}

static inline uint32_t vma_retrieve_memory_type_count(const vma_allocator* allocator) {
    return allocator->mem_props.memoryTypeCount;
}

static inline uint32_t vma_memory_type_to_heap_index(const vma_allocator* allocator, uint32_t mem_type_index) {
    assert(mem_type_index < allocator->mem_props.memoryTypeCount);
    return allocator->mem_props.memoryTypes[mem_type_index].heapIndex;
}

// True when specific memory type is HOST_VISIBLE but not HOST_COHERENT
static inline bool vma_is_memory_type_non_coherent(const vma_allocator* allocator, uint32_t mem_type_index) {
    assert(mem_type_index < allocator->mem_props.memoryTypeCount);    
    const static uint32_t non_coherency_property_bits = 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    return (allocator->mem_props.memoryTypes[mem_type_index].propertyFlags & non_coherency_property_bits) == 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
}

static inline VkDeviceSize retrieve_memory_type_min_alignment(const vma_allocator* allocator, uint32_t mem_type_index) {
    assert(mem_type_index < allocator->mem_props.memoryTypeCount);    
    VkDeviceSize default_alignment = (VkDeviceSize) VMA_DEFAULT_ALIGNMENT;
    if (vma_is_memory_type_non_coherent(allocator, mem_type_index)) {
        return default_alignment > allocator->physical_device_props.limits.nonCoherentAtomSize ?
            default_alignment : allocator->physical_device_props.limits.nonCoherentAtomSize;
    }
    return default_alignment;
}

static inline bool vma_is_integrated_gpu(const vma_allocator* allocator) {
    return allocator->physical_device_props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
}

#endif