#ifndef VULKAN_BUFFER_H
#define VULKAN_BUFFER_H

#include <stdbool.h>
#include <vulkan/vulkan.h>

#include "../../memory/memory.h"
#include "../gpu/gpu.h"
#include "../memory/allocator/vma.h"

#define VULKAN_BUFFER_MAPPED_FLAG (((VkDeviceSize) 1) << (sizeof(VkDeviceSize) \
    * 8 - 1))
#define VULKAN_BUFFER_OWNS_BUFFER_FLAG (((VkDeviceSize) 1) <<                  \
    (sizeof(VkDeviceSize) * 8 - 1))

typedef enum vk_buffer_data_usage {
    VULKAN_BUFFER_STATIC_DATA_USAGE,
    VULKAN_BUFFER_DYNAMIC_DATA_USAGE
} vk_buffer_data_usage;

typedef struct vk_buffer_create_info {
    const gpu_info* gpu;
    VkBufferUsageFlags usage;
    vk_buffer_data_usage data_usage;
    bool allocate_on_init;
    void* data;
    VkDeviceSize alloc_size;
} vk_buffer_create_info;

typedef struct vk_buffer {
    VkBuffer handle;
    const gpu_info* gpu;
    VkDeviceSize size;
    VkDeviceSize offset_in_other_buffer;
    VkBufferUsageFlags usage;
    vk_buffer_data_usage data_usage;
    vma_allocation allocation;
} vk_buffer;

static inline VkDeviceSize vk_buffer_get_allocated_size(const vk_buffer* buff) {
    return ((buff->size & ~VULKAN_BUFFER_MAPPED_FLAG) + 15) & ~15;
}

static inline VkDeviceSize vk_buffer_get_offset(const vk_buffer* buff) {
    return buff->offset_in_other_buffer & ~VULKAN_BUFFER_OWNS_BUFFER_FLAG;
}

static inline bool vk_buffer_owns_buffer(const vk_buffer* buff) {
    return (buff->offset_in_other_buffer & VULKAN_BUFFER_OWNS_BUFFER_FLAG) != 0;
}

static inline bool vk_buffer_is_mapped(const vk_buffer* buff) {
    return (buff->size & VULKAN_BUFFER_MAPPED_FLAG) != 0;
}

bool vk_buffer_copy_data(const byte* src, byte* dest, VkDeviceSize num_bytes);

void vk_buffer_init_empty(vk_buffer* buff);
bool vk_buffer_init(vk_buffer* buff, const vk_buffer_create_info* buff_conf);

bool vk_buffer_allocate(vk_buffer* buff, void* data, VkDeviceSize alloc_size);
bool vk_buffer_update_data(vk_buffer* buff, void* data, VkDeviceSize size,
    VkDeviceSize offset);

bool vk_buffer_reference(vk_buffer* dest, const vk_buffer* src);
bool vk_buffer_reference_part(vk_buffer* dest, const vk_buffer* src,
    VkDeviceSize ref_offset, VkDeviceSize ref_size);

bool vk_buffer_map(vk_buffer* buff, void** dest);
bool vk_buffer_unmap(vk_buffer* buff);

void vk_buffer_destroy(vk_buffer* buff);

#endif
