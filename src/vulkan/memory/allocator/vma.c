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
    bool status = vma_allocator_init(shwaderytine, allocator_create_info);
    ASSERT_LOG_ERROR_EXIT(status, "Unable to create allocator");
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
    mem_free(shwaderytine);
    shwaderytine = NULL;
}
