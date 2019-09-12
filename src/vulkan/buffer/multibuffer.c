#include "./multibuffer.h"

#include "../../string/string.h"

static inline vk_multibuffer_segment* vk_multibuffer_find_seg(vk_multibuffer*
    mbuff, const char name[VULKAN_MULTIBUFFER_SEGMENT_MAX_LENGTH])
{
    for (size_t i = 0; i < mbuff->segments_size; ++i) {
        if (string_equal(mbuff->segments[i].name, name)) {
            return &mbuff->segments[i];
        }
    }
    return NULL;
}

void vk_multibuffer_init_empty(vk_multibuffer* mbuff) {
    vk_buffer_init_empty(&mbuff->buffer);
    mbuff->segments = NULL;
    mbuff->segments_size = 0;
}

static inline void vk_multibuffer_segment_init(vk_multibuffer_segment* segment,
    const vk_multibuffer_segment_create_info* seg_config, VkDeviceSize offset)
{
    string_copy(segment->name, VULKAN_MULTIBUFFER_SEGMENT_MAX_LENGTH,
        seg_config->name);
    segment->pointer = 0;
    segment->size = seg_config->size;
    segment->usage = seg_config->usage;
    segment->offset = offset;
}

static inline bool vk_multibuffer_init_segments(vk_multibuffer* mbuff, const
    vk_multibuffer_segment_create_info* segments, VkDeviceSize segments_size)
{
    ASSERT_LOG_ERROR(segments_size > 0, "Unable to initialize vk_multibuffer"
        " segments - invalid segments count");
    mbuff->segments = mem_alloc(sizeof(vk_multibuffer_segment) * segments_size);
    CHECK_ALLOC_BOOL(mbuff->segments, "Unable to allocate vk_multibuffer"
        " segments");
    mbuff->segments_size = segments_size;

    VkDeviceSize offset = 0;
    for (size_t i = 0; i < mbuff->segments_size; ++i) {
        vk_multibuffer_segment_init(&mbuff->segments[i], &segments[i], offset);
        offset += segments[i].size;
        ASSERT_LOG_ERROR(is_16_byte_aligned(offset), "Vk_multibuffer invalid"
            " alignment - incorrect offset");
    }

    return true;
}

static inline bool vk_multibuffer_init_buffer(vk_multibuffer* mbuff, const
    vk_multibuffer_create_info* mbuff_config)
{
    const vk_buffer_create_info buff_config = {
        .gpu = mbuff_config->gpu,
        .usage = vk_multibuffer_get_usage(mbuff),
        .data_usage = mbuff_config->data_usage,
        .allocate_on_init = true,
        .data = NULL,
        .alloc_size = vk_multibuffer_get_total_size(mbuff)
    };
    ASSERT_LOG_ERROR(vk_buffer_init(&mbuff->buffer, &buff_config), "Unable to"
        " allocate buffer in multibuffer");

    return true;
}

static inline bool vk_multibuffer_segment_ptr_update_data(vk_multibuffer* mbuff,
    vk_multibuffer_segment* seg, const void* data, size_t data_size)
{
    ASSERT_LOG_ERROR(vk_buffer_update_data(&mbuff->buffer, data, data_size,
        seg->offset), "Unable to update vk_multibuffer_segment data: %s",
        seg->name);
    return true;
}

bool vk_multibuffer_init(vk_multibuffer* mbuff, const
    vk_multibuffer_create_info* mbuff_config)
{
    ASSERT_LOG_ERROR(mbuff_config->gpu, "Vk_multibuffer init - gpu cannot be"
        " NULL");
    vk_multibuffer_init_empty(mbuff);

    ASSERT_LOG_ERROR(vk_multibuffer_init_segments(mbuff, mbuff_config->segments,
        mbuff_config->segments_size), "Unable to init segments");
    ASSERT_LOG_ERROR(vk_multibuffer_init_buffer(mbuff, mbuff_config),
        "Unable to allocate multibuffer");

    return true;
}

VkBufferUsageFlags vk_multibuffer_get_usage(const vk_multibuffer* mbuff) {
    VkBufferUsageFlags usage = 0;
    for (size_t i = 0; i < mbuff->segments_size; ++i) {
        usage |= mbuff->segments[i].usage;
    }
    return usage;
}

VkDeviceSize vk_multibuffer_get_total_size(const vk_multibuffer* mbuff) {
    VkDeviceSize total_size = 0;
    for (size_t i = 0; i < mbuff->segments_size; ++i) {
        total_size += mbuff->segments[i].size;
    }
    return total_size;
}

VkDeviceSize vk_multibuffer_get_buffer_size(const vk_multibuffer* mbuff) {
    return vk_buffer_get_size(&mbuff->buffer);
}

bool vk_multibuffer_clear(vk_multibuffer* mbuff) {
    return vk_buffer_clear_all_data(&mbuff->buffer);
}

bool vk_multibuffer_segment_clear(vk_multibuffer* mbuff, const char
    segment_name[VULKAN_MULTIBUFFER_SEGMENT_MAX_LENGTH])
{
    vk_multibuffer_segment* seg = vk_multibuffer_find_seg(mbuff, segment_name);
    ASSERT_LOG_ERROR(seg, "Unable to find the desired segment (%s) in"
        " vk_multibuffer", segment_name);

    return vk_buffer_clear_data(&mbuff->buffer, seg->size, seg->offset);
}

bool vk_multibuffer_segment_reset(vk_multibuffer* mbuff, const char
    segment_name[VULKAN_MULTIBUFFER_SEGMENT_MAX_LENGTH], bool clear_buffer)
{
    vk_multibuffer_segment* seg = vk_multibuffer_find_seg(mbuff, segment_name);
    ASSERT_LOG_ERROR(seg, "Unable to find the desired segment (%s) in"
        " vk_multibuffer", segment_name);

    bool status = !clear_buffer || vk_multibuffer_segment_clear(mbuff,
        segment_name);
    ASSERT_LOG_ERROR(status, "Unable to clear segment (%s) in vk_multibuffer",
        segment_name);
    seg->pointer = 0;

    return true;
}

bool vk_multibuffer_segment_append(vk_multibuffer* mbuff, const char
    segment_name[VULKAN_MULTIBUFFER_SEGMENT_MAX_LENGTH], const void* data,
    size_t data_size)
{
    vk_multibuffer_segment* seg = vk_multibuffer_find_seg(mbuff, segment_name);
    ASSERT_LOG_ERROR(seg, "Unable to find the desired segment (%s) in"
        " vk_multibuffer", segment_name);

    ASSERT_LOG_ERROR(seg->pointer + data_size <= seg->size, "Unable to append"
        " data to vk_multibuffer, segment (%s) is full", segment_name);

    bool status = vk_buffer_update_data(&mbuff->buffer, data, data_size,
        seg->offset + seg->pointer);
    ASSERT_LOG_ERROR(status, "Unable to append data to desired segment (%s) in"
        " vk_multibuffer", segment_name);

    seg->pointer += data_size;

    return true;
}

bool vk_multibuffer_segment_update_data(vk_multibuffer* mbuff, const char
    segment_name[VULKAN_MULTIBUFFER_SEGMENT_MAX_LENGTH], const void* data,
    size_t data_size)
{
    vk_multibuffer_segment* seg = vk_multibuffer_find_seg(mbuff, segment_name);
    ASSERT_LOG_ERROR(seg, "Unable to find the desired segment (%s) in"
        " vk_multibuffer", segment_name);

    ASSERT_LOG_ERROR(data_size <= seg->size, "Unable to update data in segment"
        " (%s) in vk_multibuffer, not enough space", segment_name);

    bool status = vk_multibuffer_segment_clear(mbuff, segment_name)
        && vk_buffer_update_data(&mbuff->buffer, data, data_size, seg->offset);
    ASSERT_LOG_ERROR(status, "Unable to update segment (%s) data in"
        " vk_multibuffer", segment_name);

    return true;
}

void vk_multibuffer_destroy(vk_multibuffer* mbuff) {
    vk_buffer_destroy(&mbuff->buffer);
    if (mbuff->segments) {
        mem_free(mbuff->segments);
    }
    vk_multibuffer_init_empty(mbuff);
}
