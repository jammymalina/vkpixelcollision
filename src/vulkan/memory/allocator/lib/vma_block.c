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

bool vma_block_allocate(vma_block* block, vma_allocation* allocation,
    vma_block_allocation_info* block_alloc_info)
{
    return false;
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
        count++;
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
