#ifndef VMA_LIBRARY_H
#define VMA_LIBRARY_H

#include <vulkan/vulkan.h>

#include "./lib/vma_allocator.h"

void create_vma_allocator(const vma_allocator_create_info*
    allocator_create_info);
vma_allocator* retrieve_vma_allocator();
void destroy_vma_allocator();

#endif
