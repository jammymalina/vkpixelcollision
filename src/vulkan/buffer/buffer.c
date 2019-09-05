#include "./buffer.h"

void vk_buffer_init_empty(vk_buffer* buff) {
    buff->handle = VK_NULL_HANDLE;
    buff->size = 0;
    buff->offset_in_other_buffer = 0;
    buff->usage = 0;
    buff->data_usage = VULKAN_BUFFER_DYNAMIC_DATA_USAGE;
    vma_allocation_init_empty(&buff->allocation);
}

bool vk_buffer_init(vk_buffer* buff, const vk_buffer_create_info* buff_conf) {
    return true;
}

bool vk_buffer_allocate(vk_buffer* buff, void* data, VkDeviceSize alloc_size,
    VkBufferUsageFlags usage)
{
    return true;
}

bool vk_buffer_update_data(vk_buffer* buff, void* data, VkDeviceSize size,
    VkDeviceSize offset)
{
    return true;
}

bool vk_buffer_reference(vk_buffer* dest, const vk_buffer* src) {
    return true;
}

bool vk_buffer_reference_part(vk_buffer* dest, const vk_buffer* src,
    VkDeviceSize ref_offset, VkDeviceSize ref_size)
{
    return true;
}

bool vk_buffer_map(vk_buffer* buff, void* data, VkDeviceSize size, VkDeviceSize
    offset)
{
    return true;
}

bool vk_buffer_unmap(vk_buffer* buff) {
    return true;
}

void vk_buffer_destroy(vk_buffer* buff) {
}
