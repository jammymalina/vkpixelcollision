#include "./vma_allocator.h"

#include "../../../../memory/memory.h"
#include "../../../../math/math.h"

static inline bool vma_allocator_blocks_init(vma_allocator* allocator,
    const vma_allocator_create_info* allocator_info)
{
    for (size_t i = 0; i < VK_MAX_MEMORY_TYPES; ++i) {
        uint32_t min_blocks_size = max(allocator_info->min_blocks_size,
            VMA_MINIMUM_BLOCKS_SIZE);
        vector_init(&allocator->blocks[i]);
        if (!vector_reserve(&allocator->blocks[i], min_blocks_size)) {
            log_error("Unable to reserve space for allocator block vector");
            return false;
        }
    }

    return true;
}

static inline bool vma_allocator_garbage_init(vma_allocator* allocator,
    const vma_allocator_create_info* allocator_info)
{
    allocator->garbage_index = 0;
    for (size_t i = 0; i < allocator->number_of_frames; ++i) {
        uint32_t min_blocks_size = max(allocator_info->min_garbage_size,
            VMA_MINIMUM_GARBAGE_SIZE);
        vector_init(&allocator->garbage[i]);
        if (!vector_reserve(&allocator->garbage[i], min_blocks_size)) {
            log_error("Unable to reserve space for garbage vector");
            return false;
        }
    }

    return true;
}

bool vma_allocator_init(vma_allocator* allocator,
    const vma_allocator_create_info* allocator_info)
{
    ASSERT_LOG_ERROR(allocator_info->gpu, "Allocator init failed - GPU must "
        "be set");
    ASSERT_LOG_ERROR(allocator_info->gpu->device != VK_NULL_HANDLE, "Allocator "
        "init failed - logical device was not created");

    allocator->gpu = allocator_info->gpu;
    allocator->device_local_memory_bytes =
        VMA_MB_TO_BYTES(allocator_info->desired_device_local_memory_MB);
    allocator->host_visible_memory_bytes =
        VMA_MB_TO_BYTES(allocator_info->desired_host_visible_memory_MB);
    allocator->buffer_image_granularity =
        allocator_info->gpu->props.limits.bufferImageGranularity;
    allocator->number_of_frames = max(allocator_info->number_of_frames, 1);

    allocator->garbage =
        mem_alloc(sizeof(vma_allocation_vector) * allocator->number_of_frames);
    CHECK_ALLOC_BOOL(allocator->garbage, "Unable to allocate garbage vector "
        "for vma_allocator");

    if (!vma_allocator_blocks_init(allocator, allocator_info) ||
        !vma_allocator_garbage_init(allocator, allocator_info))
    {
        return false;
    }

    return true;
}

static inline void vma_allocator_blocks_destroy(vma_allocator* allocator) {
    for (size_t i = 0; i < VK_MAX_MEMORY_TYPES; ++i) {
        vma_block_vector* pv = &allocator->blocks[i];
        vma_block* block;
        vector_foreach(block, pv) {
            if (!block) { continue; }
            vma_block_destroy(block);
        }
        vector_clear(pv);
    }
}

static void vma_allocator_remove_empty_blocks(vma_allocator *allocator,
    const vma_allocation_vector* garbage_alloc_vec)
{
    for (size_t i = 0; i < garbage_alloc_vec->size; ++i) {
        vma_allocation* allocation = &garbage_alloc_vec->data[i];
        vma_block_free_allocation(allocation->block, allocation);

        if (allocation->block->allocated == 0) {
            vma_block_vector* blocks =
                &allocator->blocks[allocation->block->memory_type_index];
            ssize_t block_remove_idx = -1;
            vector_index_of(blocks, allocation->block, &block_remove_idx);
            vector_remove_noshrink(blocks, block_remove_idx);
        }
        vma_block_destroy(allocation->block);
        allocation->block = NULL;
    }
}

static inline void vma_allocator_empty_garbage_idx(vma_allocator* allocator, int
    garbage_index)
{
    vma_allocation_vector* garbage_alloc_vec =
        &allocator->garbage[garbage_index];
    vma_allocator_remove_empty_blocks(allocator, garbage_alloc_vec);
    garbage_alloc_vec->size = 0;
}

static inline void vma_allocator_garbage_destroy(vma_allocator* allocator) {
    if (!allocator->garbage) {
        return;
    }
    vma_allocator_empty_all_garbage(allocator);
    if (allocator->garbage) {
        for (size_t i = 0; i < allocator->number_of_frames; ++i) {
            vector_clear(&allocator->garbage[i]);
        }
        mem_free(allocator->garbage);
    }
    allocator->garbage = NULL;
}


void vma_allocator_destroy(vma_allocator* allocator) {
    vma_allocator_garbage_destroy(allocator);
    vma_allocator_blocks_destroy(allocator);
}

static inline uint32_t vma_count_set_bits(uint32_t n) {
    uint32_t count = 0;
    while (n) {
        count += n & 1;
        n >>= 1;
    }
    return count;
}

uint32_t vma_allocator_get_memory_type_index(const vma_allocator* allocator,
    uint32_t memory_type_bits, vma_memory_usage usage)
{
    const VkPhysicalDeviceMemoryProperties* mem_props =
        &allocator->gpu->mem_props;

    VkMemoryPropertyFlags required = 0;
    VkMemoryPropertyFlags preferred = 0;

    switch (usage) {
        case VMA_MEMORY_USAGE_GPU_ONLY:
            preferred |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            break;
        case VMA_MEMORY_USAGE_CPU_ONLY:
            required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            break;
        case VMA_MEMORY_USAGE_CPU_TO_GPU:
            required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            preferred |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            break;
        case VMA_MEMORY_USAGE_GPU_TO_CPU:
            required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            preferred |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
            break;
        default:
            log_error("Unknown memory type");
            return UINT32_MAX;
    }

    size_t best_fit = 0;
    int64_t max_score = -1;
    for (size_t i = 0; i < mem_props->memoryTypeCount; ++i) {
        if (((memory_type_bits >> i) & 1) == 0) {
            continue;
        }

        VkMemoryPropertyFlags props = mem_props->memoryTypes[i].propertyFlags;
        if ((props & required) != required) {
            continue;
        }

        if ((props & preferred) == preferred) {
            return i;
        }

        int64_t score = (int64_t) vma_count_set_bits(props & preferred);
        if (score > max_score) {
            best_fit = i;
            max_score = score;
        }
    }

    return max_score == -1 ? UINT32_MAX : best_fit;
}

static inline VkDeviceSize vma_allocator_retrieve_mem_size(const vma_allocator*
    allocator, vma_memory_usage usage)
{
    return usage == VMA_MEMORY_USAGE_GPU_ONLY ?
        allocator->device_local_memory_bytes :
        allocator->host_visible_memory_bytes;
}

static inline bool vma_allocator_allocate_new_block(vma_allocator* allocator,
    vma_allocation* alloc, const vma_allocation_create_info* alloc_info,
    uint32_t memory_type_index)
{
    VkDeviceSize blocks_size = vma_allocator_retrieve_mem_size(allocator,
        alloc_info->usage);

    vma_block* block = mem_alloc(sizeof(vma_block));
    CHECK_ALLOC_BOOL(block, "Unable to allocate vma block");
    vma_block_init_empty(block, allocator->gpu->device, memory_type_index,
        blocks_size, alloc_info->usage);

    ASSERT_LOG_ERROR(vma_block_init(block), "Unable to create vma memory "
        "block");
    ASSERT_LOG_ERROR(vector_push(&allocator->blocks[memory_type_index],
        block), "Unable to add vma memory block to blocks vector");

    const vma_block_allocation_info block_alloc_info = {
        .allocation_type = alloc_info->type,
        .align = alloc_info->align,
        .granularity = allocator->buffer_image_granularity,
        .size = alloc_info->size
    };
    return vma_block_allocate(block, alloc, &block_alloc_info);
}

static int vma_allocator_allocate_existing_block(vma_allocator*
    allocator, vma_allocation* alloc, const vma_allocation_create_info*
    alloc_info, uint32_t memory_type_index)
{
    const vma_block_vector* blocks = &allocator->blocks[memory_type_index];
    vma_block* block;
    vector_foreach(block, blocks) {
        if (block->memory_type_index == memory_type_index) {
            const vma_block_allocation_info block_alloc_info = {
                .allocation_type = alloc_info->type,
                .align = alloc_info->align,
                .granularity = allocator->buffer_image_granularity,
                .size = alloc_info->size
            };
            return vma_block_allocate(block, alloc, &block_alloc_info);
        }
    }

    return -1;
}

bool vma_allocator_allocate(vma_allocator* allocator, vma_allocation* alloc,
    const vma_allocation_create_info* alloc_info)
{
    const uint32_t memory_type_index = vma_allocator_get_memory_type_index(
        allocator, alloc_info->memory_type_bits, alloc_info->usage);

    ASSERT_LOG_ERROR(memory_type_index != UINT32_MAX, "Unable to find memory "
        "type index");

    int existing_block_status = vma_allocator_allocate_existing_block(allocator,
        alloc, alloc_info, memory_type_index);
    if (existing_block_status != -1) {
        return existing_block_status;
    }

    return vma_allocator_allocate_new_block(allocator, alloc, alloc_info,
        memory_type_index);
}

bool vma_allocator_free_allocation(vma_allocator* allocator,
    const vma_allocation* allocation)
{
    vma_allocation_vector* garbage_alloc_vec =
        &allocator->garbage[allocator->garbage_index];
    vma_allocation allocation_to_remove = *allocation;
    bool status = vector_push(garbage_alloc_vec, allocation_to_remove);
    ASSERT_LOG_ERROR(status, "Unable to add allocation to the garbage vector");
    return true;
}

void vma_allocator_empty_garbage(vma_allocator* allocator) {
    allocator->garbage_index = (allocator->garbage_index + 1) %
        allocator->number_of_frames;
    vma_allocator_empty_garbage_idx(allocator, allocator->garbage_index);
}

void vma_allocator_empty_all_garbage(vma_allocator* allocator) {
    for (int i = 0; i < allocator->number_of_frames; ++i) {
        vma_allocator_empty_garbage_idx(allocator, i);
    }
}

void vma_allocator_print_json(const vma_allocator* allocator) {
    log_info("{");
    log_info("  deviceLocalMB: %d,",
        VMA_BYTES_TO_MB(allocator->device_local_memory_bytes));
    log_info("  hostVisibleMB: %d,",
        VMA_BYTES_TO_MB(allocator->host_visible_memory_bytes));
    log_info("  bufferImageGranularity: %lu",
        allocator->buffer_image_granularity);
    log_info("}");
}
