#ifndef VULKAN_SWAPCHAIN_H
#define VULKAN_SWAPCHAIN_H

#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../gpu/gpu.h"

typedef struct vk_swapchain {
    VkSwapchainKHR handle;

    VkSurfaceFormatKHR surface_format;
    VkPresentModeKHR present_mode;
    VkExtent2D extent;

    uint32_t image_count;
} vk_swapchain;

vk_swapchain create_swapchain(const gpu_info *gpu, VkSurfaceKHR surface, const VkExtent2D *dimensions);
void destroy_swapchain(vk_swapchain *sw, VkDevice device);

#endif
