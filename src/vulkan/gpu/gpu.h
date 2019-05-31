#ifndef VULKAN_GPU_H
#define VULKAN_GPU_H

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#define DEFAULT_GRAPHICS_DEVICE_EXTENSIONS_SIZE 1
static const char* const DEFAULT_GRAPHICS_DEVICE_EXTENSIONS[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

typedef struct gpu_info {
    VkDevice device;

    VkPhysicalDevice physical_device;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceProperties props;
    VkPhysicalDeviceMemoryProperties mem_props;

    VkSurfaceCapabilitiesKHR surface_caps;

    VkSurfaceFormatKHR* surface_formats;
    uint32_t surface_formats_size;

    VkQueueFamilyProperties* queue_family_props;
    uint32_t queue_family_props_size;

    VkExtensionProperties* extension_props;
    uint32_t extension_props_size;

    VkPresentModeKHR* present_modes;
    uint32_t present_modes_size;
} gpu_info;

void init_gpu_info(gpu_info* gpu);
void init_gpu_info_props(gpu_info* gpu, VkPhysicalDevice device,
    VkSurfaceKHR surface);
void init_gpu_device(gpu_info* gpu, VkSurfaceKHR surface);

bool check_desired_extensions(const gpu_info* gpu,
    const char* const desired_extensions[], size_t desired_extensions_size);
bool is_gpu_suitable_for_graphics(const gpu_info* gpu, VkSurfaceKHR surface);
uint32_t retrieve_graphics_queue_index(const gpu_info* gpu,
    VkSurfaceKHR surface);

VkSurfaceFormatKHR retrieve_surface_format(const gpu_info* gpu);
VkPresentModeKHR retrieve_present_mode(const gpu_info* gpu);
VkExtent2D retrieve_extent(const gpu_info* gpu, const VkExtent2D* window_size);
VkFormat retrieve_supported_format(const gpu_info* gpu, VkFormat*
    formats, size_t num_formats, VkImageTiling tiling,
    VkFormatFeatureFlags features);

int default_rate_gpu(const gpu_info* gpu);

void destroy_gpu_info(gpu_info* gpu);

void destroy_device(VkDevice device);

#endif
