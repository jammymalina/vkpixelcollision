#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <vulkan/vulkan.h>

#include "../swapchain/swapchain.h"

typedef struct vk_framebuffer_create_info {
    VkDevice device;
    VkRenderPass render_pass;

    uint32_t attachment_size;
    VkImageView* attachments;

    uint32_t width;
    uint32_t height;
    uint32_t layers;
} vk_framebuffer_create_info;

typedef struct vk_framebuffer {
    VkDevice device;
    VkFramebuffer handle;
} vk_framebuffer;

void vk_framebuffer_init_empty(vk_framebuffer* fb);
bool vk_framebuffer_init(vk_framebuffer* fb, const vk_framebuffer_create_info*
    fb_info);
int vk_framebuffer_init_from_swapchain(vk_framebuffer** framebuffers, const
    vk_swapchain* swapchain, VkRenderPass render_pass);
void vk_framebuffer_destroy(vk_framebuffer* fb);

#endif
