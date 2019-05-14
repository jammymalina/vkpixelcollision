#include "./gpu_selector.h"

#include <stddef.h>
#include <stdlib.h>

#include "../../memory/memory.h"
#include "../functions/functions.h"
#include "../tools/tools.h"

bool enumerate_graphical_physical_devices(gpu_info *gpus, uint32_t *gpus_size, VkInstance instance, 
    VkSurfaceKHR surface)
{
    if (!gpus_size) {
        return false;
    }

    uint32_t num_physical_devices = 0;
    VkPhysicalDevice *physical_devices = NULL;

    CHECK_VK_BOOL(vkEnumeratePhysicalDevices(instance, &num_physical_devices, NULL));
    CHECK_VK_VAL_BOOL(num_physical_devices > 0, "No physical_devices");
    *gpus_size = num_physical_devices;

    if (!gpus) {
        return true;
    }

    physical_devices = mem_alloc(num_physical_devices * sizeof(VkPhysicalDevice));
    CHECK_ALLOC(physical_devices, "Unable to allocate enough space for physical devices");

    CHECK_VK_BOOL(vkEnumeratePhysicalDevices(instance, &num_physical_devices, physical_devices));
    CHECK_VK_VAL_BOOL(num_physical_devices > 0, "No physical devices");

    for (size_t i = 0; i < num_physical_devices; i++) {
        init_gpu_info(&gpus[i]);
        init_gpu_info_props(&gpus[i], physical_devices[i], surface);
    }

    mem_free(physical_devices);

    return true;
}

static int find_best_suitable_gpu(const gpu_info *gpus, uint32_t gpus_size, VkSurfaceKHR surface) {
    int maxScore = -1;
    int selected_gpu = -1;
    for (uint32_t i = 0; i < gpus_size; i++) {
        if (is_gpu_suitable_for_graphics(&gpus[i], surface))
        {
            int currentScore = default_rate_gpu(&gpus[i]);
            if (currentScore > maxScore) {
                selected_gpu = i;
                maxScore = currentScore;
            }
        }
    }

    return selected_gpu;
}

gpu_info select_gpu(VkInstance instance, VkSurfaceKHR surface) {
    uint32_t gpus_size = 0;

    bool enumeration_success = enumerate_graphical_physical_devices(NULL, &gpus_size, instance, surface);
    if (!enumeration_success) {
        exit(EXIT_FAILURE);
    }

    gpu_info *gpus = mem_alloc(gpus_size * sizeof(gpu_info));
    CHECK_ALLOC(gpus, "Unable to allocate space for gpus (physical devices)");

    enumerate_graphical_physical_devices(gpus, &gpus_size, instance, surface);
    if (!enumeration_success) {
        exit(EXIT_FAILURE);
    }

    int index = find_best_suitable_gpu(gpus, gpus_size, surface);
    if (index == -1) {
        log_error("Unable to find suitable gpu");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < index; i++) {
        destroy_gpu_info(&gpus[i]);
    }
    for (size_t i = index + 1; i < gpus_size; i++) {
        destroy_gpu_info(&gpus[i]);
    }

    gpu_info gpu = gpus[index];
    mem_free(gpus);

    return gpu;
}
