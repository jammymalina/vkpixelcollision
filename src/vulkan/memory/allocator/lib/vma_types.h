#ifndef VMA_TYPES_H
#define VMA_TYPES_H

#include "./vma_vector.h"

typedef struct vma_block vma_block;

typedef enum vma_memory_usage {
    VULKAN_MEMORY_USAGE_UNKNOWN,
    VULKAN_MEMORY_USAGE_GPU_ONLY,
    VULKAN_MEMORY_USAGE_CPU_ONLY,
    VULKAN_MEMORY_USAGE_CPU_TO_GPU,
    VULKAN_MEMORY_USAGE_GPU_TO_CPU,
    VULKAN_MEMORY_USAGES_COUNT
} vma_memory_usage;

typedef enum vma_allocation_type {
    VULKAN_ALLOCATION_TYPE_FREE,
    VULKAN_ALLOCATION_TYPE_BUFFER,
    VULKAN_ALLOCATION_TYPE_IMAGE,
    VULKAN_ALLOCATION_TYPE_IMAGE_LINEAR,
    VULKAN_ALLOCATION_TYPE_IMAGE_OPTIMAL,
    VULKAN_ALLOCATION_TYPES_COUNT
} vma_allocation_type;

typedef unsigned char vma_byte;

typedef struct vma_block_vector VMA_VECTOR(vma_block) vma_block_vector;

#endif
