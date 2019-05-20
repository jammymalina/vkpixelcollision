#include "./swapchain.h"

#include <stdint.h>
#include <stdlib.h>

#include "../../logger/logger.h"
#include "../../math/math.h"
#include "../functions/functions.h"
#include "../tools/tools.h"

static uint32_t retrieve_number_of_images(const gpu_info* gpu) {
    uint32_t image_count = gpu->surface_caps.minImageCount + 1;
    if (gpu->surface_caps.maxImageCount < 0) {
        return image_count;
    }
    return clamp(image_count, gpu->surface_caps.minImageCount, gpu->surface_caps.maxImageCount);
}

vk_swapchain create_swapchain(const gpu_info* gpu, VkSurfaceKHR surface, const VkExtent2D* dimensions) 
{
    vk_swapchain sw = { 0 };

    sw.surface_format = retrieve_surface_format(gpu);
    sw.present_mode = retrieve_present_mode(gpu);
    sw.extent = retrieve_extent(gpu, dimensions);

    if (sw.present_mode == VK_PRESENT_MODE_END_RANGE_KHR) {
        log_error("Unable to create swapchain, present mode not supported");
        exit(EXIT_FAILURE);
    }

    if (sw.surface_format.format == VK_FORMAT_UNDEFINED) {
        log_error("Unable to create swapchain, surface format not supported");
        exit(EXIT_FAILURE);
    }

    uint32_t image_count = retrieve_number_of_images(gpu);

    VkSwapchainCreateInfoKHR swapchain_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .surface = surface,
        .minImageCount = image_count,
        .imageFormat = sw.surface_format.format,
        .imageColorSpace = sw.surface_format.colorSpace,
        .imageExtent = sw.extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = sw.present_mode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE
    };
    
    VkSwapchainKHR swapchain;
    CHECK_VK(vkCreateSwapchainKHR(gpu->device, &swapchain_info, NULL, &swapchain));

    sw.handle = swapchain;

    return sw;
}

void destroy_swapchain(vk_swapchain* sw, VkDevice device) {
    if (sw->handle) {
        vkDestroySwapchainKHR(device, sw->handle, NULL);
    }
}