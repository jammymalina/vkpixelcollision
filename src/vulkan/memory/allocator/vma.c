#include "./vma.h"

#include <assert.h>
#include <stdlib.h>

#include "../../../logger/logger.h"
#include "../../../memory/memory.h"

static vma_allocator* shwaderytine = NULL;

vma_allocator* retrieve_vma_allocator(VkPhysicalDevice physical_device, VkDevice device) {
    if (shwaderytine != NULL) {
        return shwaderytine;
    }
    shwaderytine = mem_alloc(sizeof(vma_allocator));
    assert(shwaderytine != NULL);
    // vma_allocator_create_info alloc_create_info = {
    //     .physical_device = physical_device,
    //     .device = device
    // };
    // vma_create_allocator(shwaderytine, &alloc_create_info);

    return shwaderytine;
}