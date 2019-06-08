#include "./vma_allocation.h"

void vma_allocation_init_empty(vma_allocation* alloc) {
    alloc->id = 0;
    alloc->block = NULL;
    alloc->device_memory = VK_NULL_HANDLE;
    alloc->segment.offset = 0;
    alloc->segment.size = 0;
    alloc->data = NULL;
}
