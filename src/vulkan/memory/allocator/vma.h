#ifndef VMA_LIBRARY_H
#define VMA_LIBRARY_H

#include <vulkan/vulkan.h> 

#include "./lib/vma_allocator.h"

vma_allocator* retrieve_vma_allocator(VkPhysicalDevice physical_device, VkDevice device);

#endif