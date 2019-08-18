#ifndef RENDERING_RESOURCE
#define RENDERING_RESOURCE

#include <vulkan/vulkan.h>
#include "../vulkan/framebuffer/framebuffer.h"
#include "../vulkan/swapchain/swapchain.h"

typedef struct rendering_resource {
    VkDevice device;
    vk_framebuffer framebuffer;
    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;
    VkSemaphore image_available_semaphore;
    VkSemaphore finished_render_semaphore;
    VkFence fence;
} rendering_resource;

void rendering_resource_init_empty(rendering_resource* res);
bool rendering_resources_init_from_swapchain(rendering_resource** resources,
    const gpu_info* gpu, const vk_swapchain* swp, VkRenderPass render_pass);

void rendering_resource_destroy(rendering_resource* res);

#endif
