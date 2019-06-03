#ifndef VMA_TYPES_H
#define VMA_TYPES_H

#include "./vma_vector.h"

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


typedef unsigned char vma_byte;

typedef struct vma_block_vector VMA_VECTOR(vma_block) vma_block_vector;

#endif
