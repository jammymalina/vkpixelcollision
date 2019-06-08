#ifndef VMA_TYPES_H
#define VMA_TYPES_H

#include <stdbool.h>
#include <vulkan/vulkan.h>

#include "./vma_vector.h"

#define VMA_MB_TO_BYTES(MB_SIZE) ((MB_SIZE) << 20)
#define VMA_BYTES_TO_MB(BYTE_SIZE) ((BYTE_SIZE) >> 20)

typedef struct vma_block vma_block;
typedef struct vma_allocation vma_allocation;
typedef struct vma_allocator vma_allocator;

typedef enum vma_memory_usage {
    VMA_MEMORY_USAGE_UNKNOWN,
    VMA_MEMORY_USAGE_GPU_ONLY,
    VMA_MEMORY_USAGE_CPU_ONLY,
    VMA_MEMORY_USAGE_CPU_TO_GPU,
    VMA_MEMORY_USAGE_GPU_TO_CPU,
    VMA_MEMORY_USAGES_COUNT
} vma_memory_usage;

typedef enum vma_allocation_type {
    VMA_ALLOCATION_TYPE_FREE,
    VMA_ALLOCATION_TYPE_BUFFER,
    VMA_ALLOCATION_TYPE_IMAGE,
    VMA_ALLOCATION_TYPE_IMAGE_LINEAR,
    VMA_ALLOCATION_TYPE_IMAGE_OPTIMAL,
    VMA_ALLOCATION_TYPES_COUNT
} vma_allocation_type;

typedef struct vma_allocation_segment {
    VkDeviceSize offset;
    VkDeviceSize size;
} vma_allocation_segment;

static inline const char* vma_memory_usage_to_string(vma_memory_usage usage) {
    static const char* memory_usage_strings[VMA_MEMORY_USAGES_COUNT] = {
        "VMA_MEMORY_USAGE_UNKNOWN",
        "VMA_MEMORY_USAGE_GPU_ONLY",
        "VMA_MEMORY_USAGE_CPU_ONLY",
        "VMA_MEMORY_USAGE_CPU_TO_GPU",
        "VMA_MEMORY_USAGE_GPU_TO_CPU",
    };
    return memory_usage_strings[usage];
}

static inline const char* vma_allocation_type_to_string(
    vma_allocation_type type)
{
    static const char* allocation_type_strings[VMA_ALLOCATION_TYPES_COUNT] = {
        "VMA_ALLOCATION_TYPE_FREE",
        "VMA_ALLOCATION_TYPE_BUFFER",
        "VMA_ALLOCATION_TYPE_IMAGE",
        "VMA_ALLOCATION_TYPE_IMAGE_LINEAR",
        "VMA_ALLOCATION_TYPE_IMAGE_OPTIMAL",
    };
    return allocation_type_strings[type];
}

static inline bool vma_is_on_same_page(VkDeviceSize rA_offset,
    VkDeviceSize rA_size, VkDeviceSize rB_offset, VkDeviceSize page_size)
{
    if (!(rA_offset + rA_size <= rB_offset && rA_size > 0 && page_size > 0)) {
        return false;
    }

    VkDeviceSize rA_end = rA_offset + rA_size - 1;
    VkDeviceSize rA_end_page = rA_end & ~(page_size - 1);
    VkDeviceSize rB_start = rB_offset;
    VkDeviceSize rB_start_page = rB_start & ~(page_size - 1);

    return rA_end_page == rB_start_page;
}

static inline bool vma_has_granularity_conflict(vma_allocation_type a,
    vma_allocation_type b)
{
    if (a > b) {
        vma_allocation_type tmp = a;
        a = b;
        b = tmp;
    }

    switch (a) {
        case VMA_ALLOCATION_TYPE_FREE:
            return false;
        case VMA_ALLOCATION_TYPE_BUFFER:
            return b == VMA_ALLOCATION_TYPE_IMAGE ||
                b == VMA_ALLOCATION_TYPE_IMAGE_OPTIMAL;
        case VMA_ALLOCATION_TYPE_IMAGE:
            return b == VMA_ALLOCATION_TYPE_IMAGE ||
                b == VMA_ALLOCATION_TYPE_IMAGE_LINEAR ||
                b == VMA_ALLOCATION_TYPE_IMAGE_OPTIMAL;
        case VMA_ALLOCATION_TYPE_IMAGE_LINEAR:
            return b == VMA_ALLOCATION_TYPE_IMAGE_OPTIMAL;
        case VMA_ALLOCATION_TYPE_IMAGE_OPTIMAL:
            return false;
        default:
            return false;
    }
}

typedef unsigned char vma_byte;

typedef struct vma_block_vector VMA_VECTOR(vma_block*) vma_block_vector;
typedef struct vma_allocation_vector VMA_VECTOR(vma_allocation)
    vma_allocation_vector;

#endif
