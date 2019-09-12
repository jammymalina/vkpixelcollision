#ifndef VULKAN_MULTIBUFFER_H
#define VULKAN_MULTIBUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <vulkan/vulkan.h>

#include "../../memory/memory.h"
#include "./buffer.h"

#define VULKAN_MULTIBUFFER_SEGMENT_MAX_LENGTH 32

typedef struct vk_multibuffer_segment_create_info {
    char name[VULKAN_MULTIBUFFER_SEGMENT_MAX_LENGTH];
    VkBufferUsageFlags usage;
    VkDeviceSize size;
    const void* data;
    VkDeviceSize data_size;
} vk_multibuffer_segment_create_info;

typedef struct vk_multibuffer_segment {
    char name[VULKAN_MULTIBUFFER_SEGMENT_MAX_LENGTH];
    VkBufferUsageFlags usage;
    VkDeviceSize offset;
    VkDeviceSize size;
    VkDeviceSize pointer;
} vk_multibuffer_segment;

typedef struct vk_multibuffer_create_info {
    const gpu_info* gpu;
    const vk_multibuffer_segment_create_info* segments;
    VkDeviceSize segments_size;
    vk_buffer_data_usage data_usage;
} vk_multibuffer_create_info;

typedef struct vk_multibuffer {
    vk_buffer buffer;
    vk_multibuffer_segment* segments;
    VkDeviceSize segments_size;
} vk_multibuffer;

void vk_multibuffer_init_empty(vk_multibuffer* mbuff);
bool vk_multibuffer_init(vk_multibuffer* mbuff, const
    vk_multibuffer_create_info* mbuff_config);

VkBufferUsageFlags vk_multibuffer_get_usage(const vk_multibuffer* mbuff);
VkDeviceSize vk_multibuffer_get_total_size(const vk_multibuffer* mbuff);
VkDeviceSize vk_multibuffer_get_buffer_size(const vk_multibuffer* mbuff);

bool vk_multibuffer_clear(vk_multibuffer* mbuff);

bool vk_multibuffer_segment_clear(vk_multibuffer* mbuff, const char
    segment_name[VULKAN_MULTIBUFFER_SEGMENT_MAX_LENGTH]);
bool vk_multibuffer_segment_reset(vk_multibuffer* mbuff, const char
    segment_name[VULKAN_MULTIBUFFER_SEGMENT_MAX_LENGTH], bool clear_buffer);
bool vk_multibuffer_segment_append(vk_multibuffer* mbuff, const char
    segment_name[VULKAN_MULTIBUFFER_SEGMENT_MAX_LENGTH], const void* data,
    size_t data_size);
bool vk_multibuffer_segment_update_data(vk_multibuffer* mbuff, const char
    segment_name[VULKAN_MULTIBUFFER_SEGMENT_MAX_LENGTH], const void* data,
    size_t data_size);

void vk_multibuffer_destroy(vk_multibuffer* mbuff);

#endif
