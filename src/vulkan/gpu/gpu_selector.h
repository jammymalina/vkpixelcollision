#ifndef GPU_SELECTOR_H
#define GPU_SELECTOR_H

#include <stdbool.h>

#include "./gpu.h"

bool enumerate_graphical_physical_devices(gpu_info* gpus, uint32_t* gpus_size,
    VkInstance instance, VkSurfaceKHR surface);
int choose_suitable_graphics_gpu(const gpu_info* gpus, uint32_t gpus_size,
    VkSurfaceKHR surface);

gpu_info select_gpu(VkInstance instance, VkSurfaceKHR surface);

#endif
