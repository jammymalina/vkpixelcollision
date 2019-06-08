#include "./vma_allocator.h"

#include "../../../../memory/memory.h"
#include "../../../../math/math.h"

bool vma_allocator_init(vma_allocator* allocator,
    const vma_allocator_create_info* allocator_info)
{
    allocator->device_local_memory_bytes =
        VMA_MB_TO_BYTES(allocator_info->desired_device_local_memory_MB);
    allocator->host_visible_memory_bytes =
        VMA_MB_TO_BYTES(allocator_info->desired_host_visible_memory_MB);
    allocator->buffer_image_granularity =
        allocator_info->buffer_image_granularity;
    allocator->number_of_frames = allocator_info->number_of_frames;

    allocator->garbage =
        mem_alloc(sizeof(vma_allocation_vector) * allocator->number_of_frames);
    CHECK_ALLOC_BOOL(allocator->garbage, "Unable to allocate garbage vector"
     " for vma_allocator");

    uint32_t exp_num_alloc = max(allocator_info->expected_number_of_allocations,
        VMA_MINIMUM_EXPECTED_NUMBER_OF_ALLOCATIONS);

    for (size_t i = 0; i < VK_MAX_MEMORY_TYPES; ++i) {
        vma_vector_init(&allocator->blocks[i]);
        if (!vma_vector_reserve(&allocator->blocks[i], exp_num_alloc)) {
            log_error("Unable to reserve space for allocator block vector");
            return false;
        }
    }

    for (size_t i = 0; i < allocator->number_of_frames; ++i) {
        vma_vector_init(&allocator->garbage[i]);
    }


    return true;
}

void vma_allocator_destroy(vma_allocator* allocator) {
    if (allocator->garbage) {
        for (size_t i = 0; i < allocator->number_of_frames; ++i) {
            vma_vector_destroy(&allocator->garbage[i]);
        }
        mem_free(allocator->garbage);
    }
}

bool vma_allocator_allocate(vma_allocator* allocator, vma_allocation* alloc,
    const vma_allocation_create_info* alloc_info)
{
    return true;
}

bool vma_allocator_free_allocation(vma_allocator* allocator,
    const vma_allocation* allocation)
{
    return true;
}

void vma_allocator_empty_garbage(vma_allocator* allocator) {
}

void vma_allocator_print_json(const vma_allocator* allocator) {
}
