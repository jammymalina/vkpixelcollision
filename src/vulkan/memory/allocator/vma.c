#include "./vma.h"

#include <stdlib.h>
#include "../../../logger/logger.h"

static vma_allocator shwaderytine = {};

vma_allocator retrieve_vma_allocator(VkPhysicalDevice physical_device, VkDevice device) {
    if (shwaderytine.status.initialized) {
        return shwaderytine;
    }
    vma_allocator_create_info alloc_create_info = {
        .physical_device = physical_device,
        .device = device
    };
    shwaderytine = vma_create_allocator(&alloc_create_info);

    if (!shwaderytine.status.initialized) {
        log_error("Unable to create vulkan memory allocator: %s", shwaderytine.status.error);
        exit(EXIT_FAILURE);
    }

    return shwaderytine;
}