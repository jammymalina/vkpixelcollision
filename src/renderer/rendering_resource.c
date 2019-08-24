#include "./rendering_resource.h"

#include "../logger/logger.h"
#include "../memory/memory.h"
#include "../vulkan/functions/functions.h"
#include "../vulkan/tools/tools.h"

void rendering_resource_init_empty(rendering_resource* res) {
    res->device = VK_NULL_HANDLE;
    res->command_pool = VK_NULL_HANDLE;
    res->command_buffer = VK_NULL_HANDLE;
    res->finished_render_semaphore = VK_NULL_HANDLE;
    res->image_available_semaphore = VK_NULL_HANDLE;
    res->fence = VK_NULL_HANDLE;

    vk_framebuffer_init_empty(&res->framebuffer);
}

static bool rendering_resource_init_framebuffer_from_swp(rendering_resource*
    res, const vk_swapchain* swp, VkRenderPass render_pass, size_t swp_idx)
{
    bool status = vk_framebuffer_init_from_swapchain(&res->framebuffer, swp,
        render_pass, swp_idx);
    ASSERT_LOG_ERROR(status, "Unable to create framebuffers from swapchain");
    return true;
}

static bool rendering_resource_init_semaphores(rendering_resource* res) {
    return true;
}

static bool rendering_resource_init_fence(rendering_resource* res) {
    return true;
}

static bool rendering_resource_init_command_resources(rendering_resource* res,
    uint32_t queue_family_index)
{
    VkCommandPoolCreateInfo cmd_pool_create_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .pNext = NULL,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT |
        VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
      .queueFamilyIndex = queue_family_index
    };
    CHECK_VK_BOOL(vkCreateCommandPool(res->device, &cmd_pool_create_info, NULL,
        &res->command_pool));

    VkCommandBufferAllocateInfo command_buffer_allocate_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = res->command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };
    CHECK_VK_BOOL(vkAllocateCommandBuffers(res->device,
        &command_buffer_allocate_info, &res->command_buffer));

    return true;
}

bool rendering_resources_init_from_swapchain(rendering_resource** resources,
    const gpu_info* gpu, const vk_swapchain* swp, VkRenderPass render_pass,
    uint32_t queue_family_index)
{
    *resources = NULL;

    rendering_resource* ress = mem_alloc(sizeof(rendering_resource) *
        swp->image_count);
    CHECK_ALLOC_BOOL(ress, "Unable to allocate rendering resources");

    bool status = true;
    for (size_t i = 0; i < swp->image_count && status; ++i) {
        rendering_resource* res = &ress[i];
        rendering_resource_init_empty(res);

        res->device = gpu->device;
        status &= rendering_resource_init_framebuffer_from_swp(res, swp,
            render_pass, i) &&
            rendering_resource_init_semaphores(res) &&
            rendering_resource_init_fence(res) &&
            rendering_resource_init_command_resources(res, queue_family_index);
    }

    *resources = ress;

    return true;
}

static inline void rendering_resource_destroy_command_res(rendering_resource*
    res)
{
    if (res->command_pool) {
        vkDestroyCommandPool(res->device, res->command_pool, NULL);
        res->command_pool = VK_NULL_HANDLE;
    }
}

static inline void rendering_resource_destroy_fbs(rendering_resource* res) {
    vk_framebuffer_destroy(&res->framebuffer);
    vk_framebuffer_init_empty(&res->framebuffer);
}

void rendering_resource_destroy(rendering_resource* res) {
    rendering_resource_destroy_command_res(res);
    rendering_resource_destroy_fbs(res);
}
