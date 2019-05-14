#ifndef VULKAN_FUNCTION_LOADER_H
#define VULKAN_FUNCTION_LOADER_H

#include <vulkan/vulkan.h>

void load_external_function(PFN_vkGetInstanceProcAddr vk_get_proc);
void load_global_functions();
void load_instance_vulkan_functions(VkInstance instance);
void load_device_level_functions(VkDevice device);

#endif
