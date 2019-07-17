#include "./swapchain.h"

#include <stdint.h>
#include <stdlib.h>

#include "../../logger/logger.h"
#include "../../math/math.h"
#include "../../memory/memory.h"
#include "../functions/functions.h"
#include "../tools/tools.h"

static uint32_t retrieve_number_of_images(const gpu_info* gpu) {
    uint32_t image_count = gpu->surface_caps.minImageCount + 1;
    uint32_t max_image_count = gpu->surface_caps.maxImageCount == 0 ?
        UINT32_MAX : gpu->surface_caps.maxImageCount;
    return clamp(image_count, gpu->surface_caps.minImageCount, max_image_count);
}

void swapchain_init_empty(vk_swapchain* sw) {
    sw->handle = VK_NULL_HANDLE;
    sw->device = VK_NULL_HANDLE;
    sw->min_image_count = 0;
    sw->image_count = 0;
    sw->images = NULL;
    sw->views = NULL;
}

static bool swapchain_init_handle(vk_swapchain* sw, const
    vk_swapchain_create_info* sw_info)
{
    VkSwapchainCreateInfoKHR swapchain_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .surface = sw_info->surface,
        .minImageCount = sw->min_image_count,
        .imageFormat = sw->surface_format.format,
        .imageColorSpace = sw->surface_format.colorSpace,
        .imageExtent = sw->extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = sw->present_mode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE
    };

    VkSwapchainKHR swapchain;
    CHECK_VK_BOOL(vkCreateSwapchainKHR(sw_info->gpu->device, &swapchain_info,
        NULL, &swapchain));

    sw->handle = swapchain;

    return true;
}

static bool swapchain_init_images(vk_swapchain* sw) {
    uint32_t num_images = 0;
    CHECK_VK_BOOL(vkGetSwapchainImagesKHR(sw->device, sw->handle, &num_images,
        NULL));
    CHECK_VK_VAL_BOOL(num_images >= sw->min_image_count,
        "Number of retrieved swapchain images is incorrect");
    sw->image_count = num_images;
    sw->images = mem_alloc(sizeof(VkImage) * sw->image_count);
    CHECK_ALLOC_BOOL(sw->images, "Unable to allocate swapchain images");

    CHECK_VK_BOOL(vkGetSwapchainImagesKHR(sw->device, sw->handle,
        &num_images, sw->images));
    CHECK_VK_VAL_BOOL(num_images == sw->image_count,
        "Number of retrieved swapchain images is incorrect");

    return true;
}

static bool swapchain_init_views(vk_swapchain* sw) {
    sw->views = mem_alloc(sizeof(VkImageView) * sw->image_count);
    CHECK_ALLOC_BOOL(sw->views, "Unable to allocate swapchain images");

    for (size_t i = 0; i < sw->image_count; ++i) {
        VkImageViewCreateInfo image_view_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .image = sw->images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = sw->surface_format.format,
            .components = {
                .r = VK_COMPONENT_SWIZZLE_R,
                .g = VK_COMPONENT_SWIZZLE_G,
                .b = VK_COMPONENT_SWIZZLE_B,
                .a = VK_COMPONENT_SWIZZLE_A
            },
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        CHECK_VK_BOOL(vkCreateImageView(sw->device, &image_view_info, NULL,
            &sw->views[i]));
    }

    return true;
}

bool swapchain_init(vk_swapchain* sw, const vk_swapchain_create_info* sw_info) {
    swapchain_init_empty(sw);

    sw->device = sw_info->gpu->device;

    sw->surface_format = retrieve_surface_format(sw_info->gpu);
    sw->present_mode = retrieve_present_mode(sw_info->gpu);
    sw->extent = retrieve_extent(sw_info->gpu, &sw_info->dimensions);

    if (sw->present_mode == VK_PRESENT_MODE_END_RANGE_KHR) {
        log_error("Unable to create swapchain, present mode not supported");
        return false;
    }

    if (sw->surface_format.format == VK_FORMAT_UNDEFINED) {
        log_error("Unable to create swapchain, surface format not supported");
        return false;
    }

    sw->min_image_count = retrieve_number_of_images(sw_info->gpu);

    return swapchain_init_handle(sw, sw_info) &&
        swapchain_init_images(sw) &&
        swapchain_init_views(sw);
}

static inline void swapchain_destroy_images(vk_swapchain* sw) {
    if (sw->views) {
        for (size_t i = 0; i < sw->image_count && sw->views[i]; ++i) {
            vkDestroyImageView(sw->device, sw->views[i], NULL);
        }
    }
    if (sw->images) {
        mem_free(sw->images);
    }
}

void swapchain_destroy(vk_swapchain* sw) {
    if (!sw->handle || !sw->device) {
        return;
    }
    swapchain_destroy_images(sw);
    vkDestroySwapchainKHR(sw->device, sw->handle, NULL);
    swapchain_init_empty(sw);
}
