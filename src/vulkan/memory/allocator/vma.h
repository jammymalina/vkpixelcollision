#ifndef VMA_LIBRARY_H
#define VMA_LIBRARY_H

#include <vulkan/vulkan.h>

#include "./lib/vma_allocator.h"

vma_allocator* retrieve_vma_allocator(vma_allocator_create_info*
    allocator_create_info);
void destroy_vma_allocator();

#endif
