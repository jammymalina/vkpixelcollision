#include "./vma_block.h"

#include "../../../../logger/logger.h"
#include "../../../../memory/memory.h"
#include "../../../functions/functions.h"
#include "../../../tools/tools.h"
#include "./vma_allocation.h"

void vma_block_init_empty(vma_block* block, uint32_t memory_type_index,
    VkDeviceSize size, vma_memory_usage usage)
{
    block->head = NULL;
    block->next_block_id = 0;
    block->size = size;
    block->allocated = 0;
    block->memory_type_index = memory_type_index;
    block->usage = usage;
    block->device_memory = VK_NULL_HANDLE;
}

bool vma_block_init(vma_block* block) {
    if (block->memory_type_index == UINT64_MAX) {
        return false;
    }

    VkMemoryAllocateInfo mem_alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = block->size,
        .memoryTypeIndex = block->memory_type_index
    };

    CHECK_VK_BOOL(vkAllocateMemory(block->device, &mem_alloc_info, NULL,
        &block->device_memory));

    if (block->device_memory == VK_NULL_HANDLE) {
        return false;
    }

    if (vma_block_is_host_visible(block)) {
        CHECK_VK_BOOL(vkMapMemory(block->device, block->device_memory, 0,
            block->size, 0, (void**) &block->data));
    }

    block->head = mem_alloc(sizeof(vma_block_chunk));
    CHECK_ALLOC(block->head, "Allocation of vma_block_chunk failed");

    vma_block_chunk* head = block->head;
    head->allocation_id = block->next_block_id++;
    head->size = block->size;
    head->offset = 0;
    head->prev = NULL;
    head->next = NULL;
    head->allocation_type = VMA_ALLOCATION_TYPE_FREE;

    return true;
}

void vma_block_destroy(vma_block* block) {
    if (vma_block_is_host_visible(block)) {
        vkUnmapMemory(block->device, block->device_memory);
    }

    vkFreeMemory(block->device, block->device_memory, NULL);
    block->device_memory = VK_NULL_HANDLE;

    vma_block_chunk* prev = NULL;
    vma_block_chunk* current = block->head;

    if (current == NULL) {
        return;
    }

    while (current->next != NULL) {
        prev = current;
        current = current->next;
        mem_free(prev);
    }

    mem_free(current);
}

static inline vma_block_chunk* vma_block_chunk_find_best_fit(
    const vma_block* block, const vma_block_allocation_info* block_alloc_info)
{
    vma_block_chunk* current = NULL;
    vma_block_chunk* best_fit = NULL;
    vma_block_chunk* prev = NULL;

    VkDeviceSize padding = 0;
    VkDeviceSize offset = 0;
    VkDeviceSize aligned_size = 0;

    for (current = block->head; current != NULL; prev = current,
        current = current->next)
    {
        if (current->allocation_type != VMA_ALLOCATION_TYPE_FREE) {
            continue;
        }
        if (block_alloc_info->size > current->size) {
            continue;
        }

        offset = ALIGN(current->offset, block_alloc_info->align);

        if (prev != NULL && block_alloc_info->granularity > 1) {
            bool is_on_same_page = vma_is_on_same_page(prev->offset, prev->size,
                offset, block_alloc_info->granularity);
            bool has_granularity_conflict = vma_has_granularity_conflict(
                prev->allocation_type, block_alloc_info->allocation_type);
            if (is_on_same_page && has_granularity_conflict) {
                offset = ALIGN(offset, block_alloc_info->granularity);
            }
        }

        padding = offset - current->offset;
        aligned_size = padding + block_alloc_info->size;

        if (aligned_size > current->size) {
            continue;
        }
        if (aligned_size + block->allocated >= block->size) {
            return NULL;
        }

        if (block_alloc_info->granularity > 1 && current->next != NULL) {
            vma_block_chunk* next = current->next;
            bool is_on_same_page = vma_is_on_same_page(offset,
                block_alloc_info->size, next->offset,
                block_alloc_info->granularity);
            bool has_granularity_conflict = vma_has_granularity_conflict(
                block_alloc_info->allocation_type, next->allocation_type);
            if (is_on_same_page && has_granularity_conflict) {
                continue;
            }
        }

        best_fit = current;
        break;
    }

    return best_fit;
}

bool vma_block_allocate(vma_block* block, vma_allocation* allocation,
    const vma_block_allocation_info* block_alloc_info)
{
    const VkDeviceSize free_size = block->size - block->allocated;
    if (free_size < block_alloc_info->size) {
        return false;
    }

    return true;
}

static inline vma_block_chunk* vma_block_chunk_find_by_alloc_id(
    const vma_block *block, uint32_t alloc_id)
{
    vma_block_chunk* current = NULL;
    for (current = block->head; current != NULL &&
        current->allocation_id != alloc_id; current = current->next);
    return current;
}

void vma_block_free_allocation(vma_block* block, vma_allocation* allocation) {
    vma_block_chunk* alloc_block = vma_block_chunk_find_by_alloc_id(block,
        allocation->id);

    if (alloc_block == NULL) {
        log_warning("Trying to free unknown allocation: %u", allocation->id);
        return;
    }

    block->allocated -= allocation->size;
}

void vma_block_print_json(const vma_block* block) {
    size_t count = 0;
    for (vma_block_chunk* current = block->head;
        current != NULL; current = current->next)
    {
        ++count;
    }

    log_info("{");
    log_info("  \"typeIndex\": %u,",  block->memory_type_index);
    log_info("  \"usage\":  \"%s\",",
        vma_memory_usage_to_string(block->usage));
    log_info("  \"blocksCount\": %zu,", count);
    log_info("  \"size\": %zu,", block->size);
    log_info("  \"allocated:\"  %zu,", block->allocated);
    log_info("  \"nextBlockId\": %u,",  block->next_block_id);
    if (count == 0) {
        log_info("  \"blocks\": []");
        log_info("}");
        return;
    }
    log_info("  \"blocks\": [");

    size_t i = 0;
    for (vma_block_chunk* current = block->head; current != NULL;
        current = current->next, ++i)
    {
        log_info("    {");
        log_info("      \"allocationId\": %u", current->allocation_id);
        log_info("      \"size\": %zu,", current->size);
        log_info("      \"offset\": %zu,", current->offset);
        log_info("      \"type\": \"%s\"",
            vma_allocation_type_to_string(current->allocation_type));
        if (i < count) {
            log_info("    },");
        } else {
            log_info("    }");
        }
    }
    log_info("  ]");
    log_info("}");
}
