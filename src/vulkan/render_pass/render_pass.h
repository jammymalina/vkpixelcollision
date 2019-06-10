#ifndef RENDER_PASS_H
#define RENDER_PASS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

typedef struct vk_render_pass_create_info {
    VkDevice device;

    VkAttachmentDescription* attachment_descriptions;
    uint32_t attachment_descriptions_size;

    VkSubpassDescription* subpass_descriptons;
    uint32_t subpass_descriptons_size;

    VkSubpassDependency* subpass_dependencies;
    uint32_t subpass_dependencies_size;
} vk_render_pass_create_info;

typedef struct vk_render_pass {
    VkRenderPass handle;
    VkDevice device;
} vk_render_pass;

void render_pass_init_empty(vk_render_pass* rp);
bool render_pass_init(vk_render_pass* rp, const vk_render_pass_create_info*
    rp_info);
bool render_pass_init_swapchain_default_2d(vk_render_pass* rp, VkDevice device,
    VkFormat swapchain_format);
void render_pass_destroy(vk_render_pass* rp);

#endif
