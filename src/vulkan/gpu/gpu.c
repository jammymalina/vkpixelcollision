#include "./gpu.h"

#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../../math/math.h"
#include "../../memory/memory.h"
#include "../../string/string.h"
#include "../functions/functions.h"
#include "../tools/tools.h"

void init_gpu_info(gpu_info* gpu) {
    gpu->physical_device = VK_NULL_HANDLE;

    gpu->queue_family_props = NULL;
    gpu->queue_family_props_size = 0;

    gpu->surface_formats = NULL;
    gpu->surface_formats_size = 0;

    gpu->extension_props = NULL;
    gpu->extension_props_size = 0;

    gpu->present_modes = NULL;
    gpu->present_modes_size = 0;	
}

void init_gpu_info_props(gpu_info* gpu, VkPhysicalDevice device, VkSurfaceKHR surface) {
    gpu->physical_device = device;
    
    // Queue family props
    uint32_t num_queues = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu->physical_device, &num_queues, NULL);
    CHECK_VK_VAL(num_queues > 0, "No queue family props");
    
    gpu->queue_family_props = mem_alloc(num_queues*  sizeof(VkQueueFamilyProperties));
    CHECK_ALLOC(gpu->queue_family_props, "Allocation fail");
    
    vkGetPhysicalDeviceQueueFamilyProperties(gpu->physical_device, &num_queues, 
        gpu->queue_family_props);
    CHECK_VK_VAL(num_queues > 0, "No queue family props");
    gpu->queue_family_props_size = num_queues;
    
    // Extensions
    uint32_t num_extensions = 0; 
    CHECK_VK(vkEnumerateDeviceExtensionProperties(gpu->physical_device, NULL, &num_extensions, NULL));
    CHECK_VK_VAL(num_extensions > 0, "No device extensions");

    gpu->extension_props = mem_alloc(num_extensions*  sizeof(VkExtensionProperties));
    CHECK_ALLOC(gpu->extension_props, "Allocation fail");

    CHECK_VK(vkEnumerateDeviceExtensionProperties(gpu->physical_device, NULL, &num_extensions,
        gpu->extension_props));
    CHECK_VK_VAL(num_extensions > 0, "No device extensions");
    gpu->extension_props_size = num_extensions;

    // Surface formats
    uint32_t num_formats = 0;
    CHECK_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(gpu->physical_device, surface,
        &num_formats, NULL));
    CHECK_VK_VAL(num_formats > 0, "No surface formats");

    gpu->surface_formats = mem_alloc(num_formats*  sizeof(VkSurfaceFormatKHR));
    CHECK_ALLOC(gpu->surface_formats, "Allocation fail");

    CHECK_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(gpu->physical_device, surface,
        &num_formats, gpu->surface_formats));
    CHECK_VK_VAL(num_formats > 0, "No surface formats");
    gpu->surface_formats_size = num_formats;
    
    uint32_t num_present_modes = 0;
    CHECK_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(gpu->physical_device, surface, 
        &num_present_modes, NULL));
    CHECK_VK_VAL(num_present_modes > 0, "No surface present modes");

    gpu->present_modes = mem_alloc(num_present_modes*  sizeof(VkPresentModeKHR));
    CHECK_ALLOC(gpu->present_modes, "Allocation fail");
    
    CHECK_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(gpu->physical_device, surface, 
        &num_present_modes, gpu->present_modes));
    CHECK_VK_VAL(num_present_modes > 0, "No surface present modes");
    gpu->present_modes_size = num_present_modes;

    CHECK_VK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu->physical_device, surface, &gpu->surface_caps));
    vkGetPhysicalDeviceMemoryProperties(gpu->physical_device, &gpu->mem_props);
    vkGetPhysicalDeviceProperties(gpu->physical_device, &gpu->props);
    vkGetPhysicalDeviceFeatures(gpu->physical_device, &gpu->features);   
}

bool check_desired_extensions(const gpu_info* gpu, const char* const desired_extensions[], size_t desired_extensions_size) {
    size_t required = desired_extensions_size;
    size_t available = 0;

    for (size_t i = 0; i < desired_extensions_size; ++i) {
        for (size_t j = 0; j < gpu->extension_props_size; ++j) {
            if (string_equal(desired_extensions[i], gpu->extension_props[j].extensionName)) {
                ++available;
                break;
            }
        }
    }

    return available == required;
}

VkSurfaceFormatKHR retrieve_surface_format(const gpu_info* gpu) {
    VkSurfaceFormatKHR result = { 0 };

    if (gpu->surface_formats_size == 0) {
        result.format = VK_FORMAT_UNDEFINED;
        return result;
    }

    // If Vulkan returned an unknown format, then just force what we want.
    if (gpu->surface_formats_size == 1 && gpu->surface_formats[0].format == VK_FORMAT_UNDEFINED) {
        result.format = VK_FORMAT_B8G8R8A8_UNORM;
        result.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        return result;
    }

    // Favor 32 bit rgba and srgb nonlinear colorspace
    for (size_t i = 0; i < gpu->surface_formats_size; ++i) {
        VkSurfaceFormatKHR  *fmt = &gpu->surface_formats[i];
        if (fmt->format == VK_FORMAT_B8G8R8A8_UNORM && fmt->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            result.format = fmt->format;
            result.colorSpace = fmt->colorSpace;
            return result;
        }
    }

    // If all else fails, just return what's available 	 
    return gpu->surface_formats[0];
}

VkPresentModeKHR retrieve_present_mode(const gpu_info *gpu) {
    if (gpu->present_modes_size == 0) {
        return VK_PRESENT_MODE_END_RANGE_KHR;
    }

    const VkPresentModeKHR desired_mode = VK_PRESENT_MODE_MAILBOX_KHR;

    // Favor looking for mailbox mode.
    for (size_t i = 0; i < gpu->present_modes_size; ++i) {
        if (gpu->present_modes[i] == desired_mode) {
            return desired_mode;
        }
    }

    // If we couldn't find mailbox, then default to FIFO which is always available.
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D retrieve_extent(const gpu_info *gpu, const VkExtent2D *window_size) {
    // The extent is typically the size of the window we created the surface from.
    // However if Vulkan returns -1 then simply substitute the window size.
    VkExtent2D result = { 0 };
    if (gpu->surface_caps.currentExtent.width == -1) {
        const VkExtent2D min_extent = gpu->surface_caps.minImageExtent;
        const VkExtent2D max_extent = gpu->surface_caps.maxImageExtent;
        result.width = clamp(window_size->width, min_extent.width, max_extent.width);
        result.height = clamp(window_size->height, min_extent.height, max_extent.height);
        return result;
    }	

    return gpu->surface_caps.currentExtent;
}

VkFormat retrieve_supported_format(const gpu_info *gpu, VkFormat *formats, size_t num_formats, VkImageTiling tiling,
    VkFormatFeatureFlags features) 
{
    for (size_t i = 0; i < num_formats; ++i) {
        VkFormat format = formats[i];

        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(gpu->physical_device, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    return VK_FORMAT_UNDEFINED;
}

bool is_gpu_suitable_for_graphics(const gpu_info *gpu, VkSurfaceKHR surface) 
{
    bool sharing_mode_exclusive = false;

    if (gpu->surface_formats_size == 0) {
        return false;
    }

    if (gpu->present_modes_size == 0) {
        return false;
    }

    if (!gpu->features.geometryShader) {
        return false;
    }

    if (!(gpu->surface_caps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)) {
        return false;
    }

    if (!check_desired_extensions(gpu, DEFAULT_GRAPHICS_DEVICE_EXTENSIONS,
        DEFAULT_GRAPHICS_DEVICE_EXTENSIONS_SIZE)) 
    {
        return false;
    }

    for (uint32_t i = 0; i < gpu->queue_family_props_size; ++i) {
        VkQueueFamilyProperties *props = &gpu->queue_family_props[i];
        if (props->queueCount == 0) {
            continue;
        }

        bool graphics_supported = props->queueFlags & VK_QUEUE_GRAPHICS_BIT;

        VkBool32 support_present = VK_FALSE;
        CHECK_VK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu->physical_device, i, surface, &support_present));
        if (support_present && graphics_supported) {
            sharing_mode_exclusive = true;
        }
    }

    return sharing_mode_exclusive;
}

uint32_t retrieve_graphics_queue_index(const gpu_info *gpu, VkSurfaceKHR surface) {
    for (uint32_t i = 0; i < gpu->queue_family_props_size; ++i) {
        VkQueueFamilyProperties *props = &gpu->queue_family_props[i];
        if (props->queueCount == 0) {
            continue;
        }

        bool graphics_supported = props->queueFlags & VK_QUEUE_GRAPHICS_BIT;

        VkBool32 support_present = VK_FALSE;
        CHECK_VK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu->physical_device, i, surface, &support_present));
        if (support_present && graphics_supported) {
            return i;
        }
    }

    return UINT32_MAX;
}

int default_rate_gpu(const gpu_info *gpu) {
    int score = 0;

    if (gpu->props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    return score;
}

void destroy_gpu_info(gpu_info *gpu) {
    if (gpu->queue_family_props) {
        mem_free(gpu->queue_family_props);
        gpu->queue_family_props = NULL;
        gpu->queue_family_props_size = 0;
    }
    if (gpu->extension_props) {
        mem_free(gpu->extension_props);
        gpu->extension_props = NULL;
        gpu->extension_props_size = 0;
    }
    if (gpu->surface_formats) {
        mem_free(gpu->surface_formats);
        gpu->surface_formats = NULL;
        gpu->surface_formats_size = 0;
    }
    if (gpu->present_modes) {
        mem_free(gpu->present_modes);
        gpu->present_modes = NULL;
        gpu->present_modes_size = 0;
    }
    if (gpu->device) {
        destroy_device(gpu->device);
        gpu->device = VK_NULL_HANDLE;
    }
}

void init_gpu_device(gpu_info *gpu, VkSurfaceKHR surface) {
    uint32_t graphics_index = retrieve_graphics_queue_index(gpu, surface);

    uint32_t queue_count = 1;
    const float priority = 1.0f;
    VkDeviceQueueCreateInfo qinfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = NULL,
        .queueFamilyIndex = graphics_index,
        .queueCount = 1,
        .pQueuePriorities = &priority
    };

    VkPhysicalDeviceFeatures device_features = {};
    device_features.textureCompressionBC = VK_TRUE;
    device_features.imageCubeArray = VK_TRUE;
    device_features.depthClamp = VK_TRUE;
    device_features.depthBiasClamp = VK_TRUE;
    device_features.depthBounds = gpu->features.depthBounds;
    device_features.fillModeNonSolid = VK_TRUE;

    VkDeviceCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .queueCreateInfoCount = queue_count,
        .pQueueCreateInfos = &qinfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = DEFAULT_GRAPHICS_DEVICE_EXTENSIONS_SIZE,
        .ppEnabledExtensionNames = DEFAULT_GRAPHICS_DEVICE_EXTENSIONS,
        .pEnabledFeatures = &device_features
    };

    VkDevice device;
    CHECK_VK(vkCreateDevice(gpu->physical_device, &info, NULL, &device));

    gpu->device = device;
}

void destroy_device(VkDevice device) {
    vkDestroyDevice(device, NULL);
}
