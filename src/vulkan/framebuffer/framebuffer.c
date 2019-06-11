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

    VkFramebufferCreateInfo framebuffer_info = {
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

int vk_framebuffer_init_from_swapchain(vk_framebuffer** framebuffers, const
    vk_swapchain* swapchain, VkRenderPass render_pass)
{
    *framebuffers = NULL;
    vk_framebuffer* fbs = mem_alloc(sizeof(vk_framebuffer) *
        swapchain->image_count);
    if (!fbs) {
        return 0;
    }
    int success_fb_init_count = 0;
    for (size_t i = 0; i < swapchain->image_count; ++i) {
        const vk_framebuffer_create_info fb_info = {
            .device = swapchain->device,
            .render_pass = render_pass,
            .attachment_size = swapchain->image_count,
            .attachments = swapchain->views,
            .width = swapchain->extent.width,
            .height = swapchain->extent.height,
            .layers = 1
        };
        if (!vk_framebuffer_init(&fbs[i], &fb_info)) {
            break;
        }
        ++success_fb_init_count;
    }
    *framebuffers = fbs;

    return success_fb_init_count;
}

void vk_framebuffer_destroy(vk_framebuffer* fb) {
    if (!fb->handle) {
        return;
    }
    vkDestroyFramebuffer(fb->device, fb->handle, NULL);
}
