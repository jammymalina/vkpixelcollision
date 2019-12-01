#include "./buffer.h"

#include "../../logger/logger.h"
#include "../functions/functions.h"
#include "../tools/tools.h"

static inline void vk_buffer_set_mapped(vk_buffer *buffer) {
    buffer->size |= VULKAN_BUFFER_MAPPED_FLAG;
}

static inline void vk_buffer_set_unmapped(vk_buffer *buffer) {
    buffer->size &= ~VULKAN_BUFFER_MAPPED_FLAG;
}

void vk_buffer_init_empty(vk_buffer* buff) {
    buff->handle = VK_NULL_HANDLE;
    buff->gpu = NULL;
    buff->size = 0;
    buff->offset_in_other_buffer = VULKAN_BUFFER_OWNS_BUFFER_FLAG;
    buff->usage = 0;
    buff->data_usage = VULKAN_BUFFER_DYNAMIC_DATA_USAGE;
    vk_buffer_set_unmapped(buff);
    vma_allocation_init_empty(&buff->allocation);
}

bool vk_buffer_init(vk_buffer* buff, const vk_buffer_create_info* buff_conf) {
    vk_buffer_init_empty(buff);

    buff->usage = buff_conf->usage;
    buff->data_usage = buff_conf->data_usage;
    buff->gpu = buff_conf->gpu;

    if (buff_conf->allocate_on_init) {
        return vk_buffer_allocate(buff, buff_conf->data,
            buff_conf->alloc_size);
    }

    return true;
}

static inline vma_memory_usage vk_buffer_data_usage_to_memory_usage(
    vk_buffer_data_usage data_usage)
{
    return data_usage == VULKAN_BUFFER_STATIC_DATA_USAGE ?
        VMA_MEMORY_USAGE_GPU_ONLY : VMA_MEMORY_USAGE_CPU_TO_GPU;
}

static bool vk_buffer_allocate_memory(vk_buffer* buff) {
    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(buff->gpu->device, buff->handle,
        &mem_requirements);

    vma_allocator* allocator = retrieve_vma_allocator();
    if (!allocator) {
        log_error("Vk_buffer allocation failed - unable to retrieve allocator");
        return false;
    }

    vma_allocation_create_info alloc_info = {
        .type = VMA_ALLOCATION_TYPE_BUFFER,
        .size = mem_requirements.size,
        .align = mem_requirements.alignment,
        .memory_type_bits = mem_requirements.memoryTypeBits,
        .usage = vk_buffer_data_usage_to_memory_usage(buff->data_usage)
    };

    ASSERT_LOG_ERROR(vma_allocator_allocate(allocator, &buff->allocation,
        &alloc_info), "vma_allocator_allocate failed");

    return true;
}

bool vk_buffer_allocate(vk_buffer* buff, void* data, VkDeviceSize alloc_size)
{
    ASSERT_LOG_WARNING(!buff->handle, "Vk_buffer already allocated");
    ASSERT_LOG_ERROR(buff->gpu, "Vk_buffer no gpu set");
    ASSERT_LOG_ERROR(alloc_size != 0, "Unable to allocate vk_buffer of size 0");

    buff->size = alloc_size;

    VkDeviceSize num_bytes = vk_buffer_get_allocated_size(buff);

    if (buff->data_usage == VULKAN_BUFFER_STATIC_DATA_USAGE) {
        buff->usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }

    VkBufferCreateInfo buffer_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .size = num_bytes,
        .usage = buff->usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL
    };

    CHECK_VK_BOOL(vkCreateBuffer(buff->gpu->device, &buffer_info, NULL,
        &buff->handle));

    ASSERT_LOG_ERROR(vk_buffer_allocate_memory(buff),
        "Unable to allocate memory for vk_buffer");

    CHECK_VK_BOOL(vkBindBufferMemory(buff->gpu->device, buff->handle,
        buff->allocation.device_memory, buff->allocation.segment.offset));

    if (data) {
        ASSERT_LOG_ERROR(vk_buffer_update_data(buff, data, alloc_size, 0),
            "Unable to update data in vk_buffer");
    }

    return true;
}

static inline bool vk_buffer_copy_data(const byte* src, byte* dest, VkDeviceSize
    num_bytes)
{
    ASSERT_LOG_ERROR(is_16_byte_aligned(src), "Vk_buffer src data incorrect"
        " alignment");
    ASSERT_LOG_ERROR(is_16_byte_aligned(dest), "Vk_buffer dest data incorrect"
        " alignment");

    mem_copy(dest, src, num_bytes);

    return true;
}

static inline void vk_buffer_copy_data_unsafe(const byte* src, byte* dest,
    VkDeviceSize num_bytes)
{
    mem_copy(dest, src, num_bytes);
}

bool vk_buffer_clear_all_data(vk_buffer* buff) {
    return vk_buffer_clear_data(buff, 0, vk_buffer_get_size(buff));
}

bool vk_buffer_clear_data(vk_buffer* buff, VkDeviceSize size, VkDeviceSize
    offset)
{
    ASSERT_LOG_ERROR(buff->handle, "Vk_buffer must be allocated before data"
        " purge");
    ASSERT_LOG_ERROR((vk_buffer_get_offset(buff) & 15) == 0, "Vk_buffer data"
        " clear failed: invalid offset");
    ASSERT_LOG_ERROR(size <= vk_buffer_get_size(buff), "Vk_buffer not enough"
        " space to clear the data");

    if (buff->data_usage == VULKAN_BUFFER_DYNAMIC_DATA_USAGE) {
        mem_set(buff->allocation.data + vk_buffer_get_offset(buff) + offset, 0,
            size);
        return true;
    }

    log_error("Vk_buffer data usage not supported");
    return false;
}

bool vk_buffer_update_data(vk_buffer* buff, const void* data, VkDeviceSize size,
    VkDeviceSize offset)
{
    ASSERT_LOG_ERROR(buff->handle, "Vk_buffer must be allocated before data"
        " update");
    ASSERT_LOG_ERROR((vk_buffer_get_offset(buff) & 15) == 0, "Vk_buffer data"
        " update failed: invalid offset");
    ASSERT_LOG_ERROR(size <= vk_buffer_get_size(buff), "Vk_buffer not enough"
        " space to update the data");

    if (buff->data_usage == VULKAN_BUFFER_DYNAMIC_DATA_USAGE) {
        ASSERT_LOG_ERROR(vk_buffer_copy_data(data, buff->allocation.data +
            vk_buffer_get_offset(buff) + offset, size), "Vk_buffer unable to"
            " copy data");
        return true;
    }

    log_error("Vk_buffer data usage not supported");
    return false;
}

bool vk_buffer_update_data_unsafe(vk_buffer* buff, const void* data,
    VkDeviceSize size, VkDeviceSize offset)
{
    ASSERT_LOG_ERROR(buff->handle, "Vk_buffer must be allocated before data"
        " update");
    ASSERT_LOG_ERROR(size <= vk_buffer_get_size(buff), "Vk_buffer not enough"
        " space to update the data");

    if (buff->data_usage == VULKAN_BUFFER_DYNAMIC_DATA_USAGE) {
        vk_buffer_copy_data_unsafe(data, buff->allocation.data +
            vk_buffer_get_offset(buff) + offset, size);
        return true;
    }

    log_error("Vk_buffer data usage not supported");
    return false;
}

bool vk_buffer_reference(vk_buffer* dest, const vk_buffer* src) {
    ASSERT_LOG_ERROR(!vk_buffer_is_mapped(dest), "Vk_buffer is mapped, unable"
        " to create reference");

    vk_buffer_destroy(dest);
    dest->size = src->size;
    dest->offset_in_other_buffer = vk_buffer_get_offset(src);
    dest->usage = src->usage;
    dest->handle = src->handle;
    dest->allocation = src->allocation;

    ASSERT_LOG_ERROR(!vk_buffer_owns_buffer(dest), "Vk_buffer should be a"
        " reference and not own itself");

    return true;
}

bool vk_buffer_reference_part(vk_buffer* dest, const vk_buffer* src,
    VkDeviceSize ref_offset, VkDeviceSize ref_size)
{
    ASSERT_LOG_ERROR(!vk_buffer_is_mapped(dest), "Vk_buffer is mapped, unable"
        " to create reference");
    ASSERT_LOG_ERROR(ref_offset + ref_size <= src->size, "Unable to reference"
        " buffer, the reference part is out of bounds of the source buffer");

    vk_buffer_destroy(dest);
    dest->size = ref_size;
    dest->offset_in_other_buffer = vk_buffer_get_offset(src) + ref_offset;
    dest->usage = src->usage;
    dest->handle = src->handle;
    dest->allocation = src->allocation;

    ASSERT_LOG_ERROR(!vk_buffer_owns_buffer(dest), "Vk_buffer should be a"
        " reference and not own itself");

    return true;
}

bool vk_buffer_map(vk_buffer* buff, void** dest) {
    ASSERT_LOG_ERROR(buff->handle, "Vk_buffer must be allocated before data"
        " mapping");
    ASSERT_LOG_ERROR(buff->data_usage != VULKAN_BUFFER_STATIC_DATA_USAGE,
        "Static vk_buffer cannot be mapped");
    ASSERT_LOG_ERROR(buff->allocation.data, "Vk_buffer is not allocated");

    void* data = buff->allocation.data + vk_buffer_get_allocated_size(buff);

    vk_buffer_set_mapped(buff);
    ASSERT_LOG_ERROR(data, "Vk_buffer mapping of data failed");
    *dest = data;

    return true;
}

bool vk_buffer_unmap(vk_buffer* buff) {
    ASSERT_LOG_ERROR(buff->handle, "Vk_buffer must be allocated before data"
        " unmapping");
    ASSERT_LOG_ERROR(buff->data_usage != VULKAN_BUFFER_STATIC_DATA_USAGE,
        "Static vk_buffer cannot be unmapped");

    vk_buffer_set_unmapped(buff);

    return true;
}

void vk_buffer_destroy(vk_buffer* buff) {
    if (vk_buffer_is_mapped(buff)) {
        vk_buffer_unmap(buff);
    }
    if (!vk_buffer_owns_buffer(buff)) {
        vk_buffer_init_empty(buff);
        return;
    }
    if (!buff->handle) {
        return;
    }

    vma_allocator* allocator = retrieve_vma_allocator();
    vma_allocator_free_allocation(allocator, &buff->allocation);
    vkDestroyBuffer(buff->gpu->device, buff->handle, NULL);
    vk_buffer_init_empty(buff);
}
