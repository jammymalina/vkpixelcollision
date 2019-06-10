#ifndef VULKAN_SWAPCHAIN_H
#define VULKAN_SWAPCHAIN_H

#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../gpu/gpu.h"

typedef struct vk_swapchain_create_info {
    VkSurfaceKHR surface;
    VkExtent2D dimensions;
    gpu_info* gpu;
} vk_swapchain_create_info;

typedef struct vk_swapchain {
    VkSwapchainKHR handle;
    VkDevice device;

    VkSurfaceFormatKHR surface_format;
    VkPresentModeKHR present_mode;
    VkExtent2D extent;

    uint32_t min_image_count;
    uint32_t image_count;
    VkImage* images;
    VkImageView* views;
} vk_swapchain;

void swapchain_init_empty(vk_swapchain* sw);
bool swapchain_init(vk_swapchain* sw, const vk_swapchain_create_info* sw_info);
void swapchain_destroy(vk_swapchain* sw);

#endif
