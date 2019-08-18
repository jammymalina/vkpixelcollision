#include "./framebuffer.h"

#include "../../memory/memory.h"
#include "../functions/functions.h"
#include "../tools/tools.h"

void vk_framebuffer_init_empty(vk_framebuffer* fb) {
    fb->handle = VK_NULL_HANDLE;
    fb->device = VK_NULL_HANDLE;
}

bool vk_framebuffer_init(vk_framebuffer* fb, const vk_framebuffer_create_info*
    fb_info)
{
    vk_framebuffer_init_empty(fb);
    fb->device = fb_info->device;

    const VkFramebufferCreateInfo framebuffer_info = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .renderPass = fb_info->render_pass,
        .attachmentCount = fb_info->attachment_size,
        .pAttachments = fb_info->attachments,
        .width = fb_info->width,
        .height = fb_info->height,
        .layers = fb_info->layers
    };
    CHECK_VK_BOOL(vkCreateFramebuffer(fb->device, &framebuffer_info,
        NULL, &fb->handle));

    return true;
}

bool vk_framebuffer_init_from_swapchain(vk_framebuffer* fb, const vk_swapchain*
    swapchain, VkRenderPass render_pass, size_t swapchain_view_idx)
{
    vk_framebuffer_init_empty(fb);
    const vk_framebuffer_create_info fb_info = {
        .device = swapchain->device,
        .render_pass = render_pass,
        .attachment_size = 1,
        .attachments = &swapchain->views[swapchain_view_idx],
        .width = swapchain->extent.width,
        .height = swapchain->extent.height,
        .layers = 1
    };
    return vk_framebuffer_init(fb, &fb_info);
}

void vk_framebuffer_destroy(vk_framebuffer* fb) {
    if (!fb->handle) {
        return;
    }
    vkDestroyFramebuffer(fb->device, fb->handle, NULL);
}
