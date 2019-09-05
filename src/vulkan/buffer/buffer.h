#ifndef VULKAN_BUFFER_H
#define VULKAN_BUFFER_H

#include <vulkan/vulkan.h>

#include "../memory/allocator/vma.h"

typedef struct vk_buffer {
    VkBuffer handle;
    VkDeviceSize size;
    VkDeviceSize offset_in_other_buffer;
    VkBufferUsageFlags usage;
} vk_buffer;

#endif
