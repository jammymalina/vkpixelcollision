#include "./vma.h"

#include <assert.h>
#include <stdlib.h>

#include "../../../logger/logger.h"
#include "../../../memory/memory.h"

static vma_allocator* shwaderytine = NULL;

void create_vma_allocator(const vma_allocator_create_info*
    allocator_create_info)
{
    shwaderytine = mem_alloc(sizeof(vma_allocator));
    CHECK_ALLOC(shwaderytine, "Unable to create allocator - mem alloc issue");
    if (!vma_allocator_init(shwaderytine, allocator_create_info)) {
        log_error("Unable to create allocator");
        exit(EXIT_FAILURE);
    }
}

vma_allocator* retrieve_vma_allocator()
{
    return shwaderytine;
}

void destroy_vma_allocator() {
    if (!shwaderytine) {
        return;
    }
    vma_allocator_destroy(shwaderytine);
    // shwaderytine = NULL;
}
