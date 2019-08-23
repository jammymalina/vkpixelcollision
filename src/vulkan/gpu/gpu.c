#include "./gpu.h"

#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../../math/math.h"
#include "../../memory/memory.h"
#include "../../string/string.h"
#include "../functions/functions.h"
#include "../tools/tools.h"
#include "./gpu_queue_selector.h"

void gpu_info_init_empty(gpu_info* gpu) {
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


static bool gpu_info_init_queue_family_props(gpu_info* gpu) {
    uint32_t num_queues = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu->physical_device, &num_queues,
        NULL);
    CHECK_VK_VAL_BOOL(num_queues > 0, "No queue family props");

    gpu->queue_family_props = mem_alloc(num_queues *
        sizeof(VkQueueFamilyProperties));
    CHECK_ALLOC_BOOL(gpu->queue_family_props, "Allocation fail");

    vkGetPhysicalDeviceQueueFamilyProperties(gpu->physical_device, &num_queues,
        gpu->queue_family_props);
    CHECK_VK_VAL_BOOL(num_queues > 0, "No queue family props");
    gpu->queue_family_props_size = num_queues;

    return true;
}

static bool gpu_info_init_extensions(gpu_info* gpu) {
    uint32_t num_extensions = 0;
    CHECK_VK_BOOL(vkEnumerateDeviceExtensionProperties(gpu->physical_device,
        NULL, &num_extensions, NULL));
    CHECK_VK_VAL_BOOL(num_extensions > 0, "No device extensions");

    gpu->extension_props = mem_alloc(num_extensions *
        sizeof(VkExtensionProperties));
    CHECK_ALLOC_BOOL(gpu->extension_props, "Allocation fail");

    CHECK_VK_BOOL(vkEnumerateDeviceExtensionProperties(gpu->physical_device, NULL,
        &num_extensions, gpu->extension_props));
    CHECK_VK_VAL_BOOL(num_extensions > 0, "No device extensions");
    gpu->extension_props_size = num_extensions;

    return true;
}

static bool gpu_info_init_surface_formats(gpu_info* gpu, VkSurfaceKHR surface) {
    uint32_t num_formats = 0;
    CHECK_VK_BOOL(vkGetPhysicalDeviceSurfaceFormatsKHR(gpu->physical_device, surface,
        &num_formats, NULL));
    CHECK_VK_VAL_BOOL(num_formats > 0, "No surface formats");

    gpu->surface_formats = mem_alloc(num_formats * sizeof(VkSurfaceFormatKHR));
    CHECK_ALLOC_BOOL(gpu->surface_formats, "Allocation fail");

    CHECK_VK_BOOL(vkGetPhysicalDeviceSurfaceFormatsKHR(gpu->physical_device, surface,
        &num_formats, gpu->surface_formats));
    CHECK_VK_VAL_BOOL(num_formats > 0, "No surface formats");
    gpu->surface_formats_size = num_formats;

    return true;
}

static bool gpu_info_init_surface_present_modes(gpu_info* gpu, VkSurfaceKHR
    surface)
{
    uint32_t num_present_modes = 0;
    CHECK_VK_BOOL(vkGetPhysicalDeviceSurfacePresentModesKHR(gpu->physical_device,
        surface, &num_present_modes, NULL));
    CHECK_VK_VAL_BOOL(num_present_modes > 0, "No surface present modes");

    gpu->present_modes = mem_alloc(num_present_modes *
        sizeof(VkPresentModeKHR));
    CHECK_ALLOC_BOOL(gpu->present_modes, "Allocation fail");

    CHECK_VK_BOOL(vkGetPhysicalDeviceSurfacePresentModesKHR(
        gpu->physical_device, surface, &num_present_modes, gpu->present_modes));
    CHECK_VK_VAL_BOOL(num_present_modes > 0, "No surface present modes");
    gpu->present_modes_size = num_present_modes;

    return true;
}

bool gpu_info_props_init(gpu_info* gpu, VkPhysicalDevice device,
    VkSurfaceKHR surface)
{
    gpu->physical_device = device;

    bool status = gpu_info_init_queue_family_props(gpu) &&
        gpu_info_init_extensions(gpu) &&
        gpu_info_init_surface_formats(gpu, surface) &&
        gpu_info_init_surface_present_modes(gpu, surface);
    ASSERT_LOG_ERROR(status, "Unable to initialize gpu properties");

    CHECK_VK_BOOL(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        gpu->physical_device, surface, &gpu->surface_caps));
    vkGetPhysicalDeviceMemoryProperties(gpu->physical_device, &gpu->mem_props);
    vkGetPhysicalDeviceProperties(gpu->physical_device, &gpu->props);
    vkGetPhysicalDeviceFeatures(gpu->physical_device, &gpu->features);

    return true;
}

bool gpu_info_sort_queue_families(gpu_info* gpu) {
    ASSERT_LOG_ERROR(gpu->queue_family_props_size > 0, "Unable to sort queue"
        " indices");
    return true;
}

bool check_desired_extensions(const gpu_info* gpu, const char* const
    desired_extensions[], size_t desired_extensions_size)
{
    size_t required = desired_extensions_size;
    size_t available = 0;

    for (size_t i = 0; i < desired_extensions_size; ++i) {
        for (size_t j = 0; j < gpu->extension_props_size; ++j) {
            if (string_equal(desired_extensions[i],
                gpu->extension_props[j].extensionName))
            {
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

    // If Vulkan returned an unknown format, then just force what we want
    if (gpu->surface_formats_size == 1 && gpu->surface_formats[0].format ==
        VK_FORMAT_UNDEFINED)
    {
        result.format = VK_FORMAT_B8G8R8A8_UNORM;
        result.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        return result;
    }

    // Favor 32 bit rgba and srgb nonlinear colorspace
    for (size_t i = 0; i < gpu->surface_formats_size; ++i) {
        VkSurfaceFormatKHR  *fmt = &gpu->surface_formats[i];
        if (fmt->format == VK_FORMAT_B8G8R8A8_UNORM && fmt->colorSpace ==
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
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

    // If we couldn't find mailbox, then default to FIFO which is always
    // available
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D retrieve_extent(const gpu_info *gpu, const VkExtent2D *window_size) {
    // The extent is typically the size of the window we created the surface
    // from. However if Vulkan returns -1 then simply substitute the window
    // size
    VkExtent2D result = { 0 };
    if (gpu->surface_caps.currentExtent.width == -1) {
        const VkExtent2D min_extent = gpu->surface_caps.minImageExtent;
        const VkExtent2D max_extent = gpu->surface_caps.maxImageExtent;
        result.width = clamp(window_size->width, min_extent.width,
            max_extent.width);
        result.height = clamp(window_size->height, min_extent.height,
            max_extent.height);
        return result;
    }

    return gpu->surface_caps.currentExtent;
}

VkFormat retrieve_supported_format(const gpu_info *gpu, VkFormat *formats,
    size_t num_formats, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (size_t i = 0; i < num_formats; ++i) {
        VkFormat format = formats[i];

        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(gpu->physical_device, format,
            &props);

        if (tiling == VK_IMAGE_TILING_LINEAR &&
            (props.linearTilingFeatures & features) == features)
        {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
            (props.optimalTilingFeatures & features) == features)
        {
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

    if (!(gpu->surface_caps.supportedUsageFlags &
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT))
    {
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
        CHECK_VK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu->physical_device, i,
            surface, &support_present));
        if (support_present && graphics_supported) {
            sharing_mode_exclusive = true;
        }
    }

    return sharing_mode_exclusive;
}

int default_rate_gpu(const gpu_info *gpu) {
    int score = 0;

    if (gpu->props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    return score;
}

void gpu_info_destroy(gpu_info *gpu) {
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

bool gpu_device_init(gpu_info *gpu, VkSurfaceKHR surface) {
    const gpu_queue_selector* selector = retrieve_gpu_queue_selector();
    ASSERT_LOG_ERROR(selector, "GPU selector must be initialized");

    const uint32_t queue_family_count =
        gpu_selector_get_used_queue_family_count(selector);
    ASSERT_LOG_ERROR(queue_family_count > 0, "No queues to select");
    const VkDeviceQueueCreateInfo* queues_infos =
        mem_alloc(sizeof(VkDeviceCreateInfo) * queue_family_count);
    CHECK_ALLOC_BOOL(queues_infos, "Unable to allocate device queues create"
        " config");
    bool status = gpu_selector_get_device_queue_create_info(selector,
        queues_infos);
    ASSERT_LOG_ERROR(status, "Unable to retrieve device queues create info");

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
        .queueCreateInfoCount = queue_family_count,
        .pQueueCreateInfos = &queues_infos,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = DEFAULT_GRAPHICS_DEVICE_EXTENSIONS_SIZE,
        .ppEnabledExtensionNames = DEFAULT_GRAPHICS_DEVICE_EXTENSIONS,
        .pEnabledFeatures = &device_features
    };

    VkDevice device;
    CHECK_VK_BOOL(vkCreateDevice(gpu->physical_device, &info, NULL, &device));

    gpu_selector_destroy_device_queue_create_info(queues_infos,
        queue_family_count);
    mem_free(queues_infos);

    gpu->device = device;

    return true;
}

void destroy_device(VkDevice device) {
    vkDestroyDevice(device, NULL);
}
