#include "./render_pass.h"

#include "../functions/functions.h"
#include "../tools/tools.h"

void render_pass_init_empty(vk_render_pass *rp) {
    rp->handle = VK_NULL_HANDLE;
    rp->device = VK_NULL_HANDLE;
}

bool render_pass_init(vk_render_pass* rp, const vk_render_pass_create_info*
    rp_info)
{
    render_pass_init_empty(rp);
    rp->device = rp_info->device;

    VkRenderPassCreateInfo render_pass_info = {
        .sType = 0,
        .pNext = NULL,
        .flags = 0,
        .attachmentCount = rp_info->attachment_descriptions_size,
        .pAttachments = rp_info->attachment_descriptions,
        .subpassCount = rp_info->subpass_descriptons_size,
        .pSubpasses = rp_info->subpass_descriptons,
        .dependencyCount = rp_info->subpass_dependencies_size,
        .pDependencies = rp_info->subpass_dependencies
    };

    CHECK_VK_BOOL(vkCreateRenderPass(rp_info->device, &render_pass_info, NULL,
        &rp->handle));

    return true;
}

bool render_pass_init_swapchain_default_2d(vk_render_pass* rp, VkDevice device,
    VkFormat swapchain_format)
{
    VkAttachmentDescription color_attachment = {
        .flags = 0,
        .format = swapchain_format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    VkAttachmentReference color_ref = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpass = {
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = NULL,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_ref,
        .pResolveAttachments = NULL,
        .pDepthStencilAttachment = NULL,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = NULL
    };

    VkAttachmentDescription attachments[] = { color_attachment };

    VkSubpassDependency subpass_dependencies[] = {
        {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
        },
        {
            .srcSubpass = 0,
            .dstSubpass = VK_SUBPASS_EXTERNAL,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
        }
    };

    vk_render_pass_create_info rp_info = {
        .device = device,
        .attachment_descriptions = attachments,
        .attachment_descriptions_size = 1,
        .subpass_descriptons = &subpass,
        .subpass_descriptons_size = 1,
        .subpass_dependencies = subpass_dependencies,
        .subpass_dependencies_size = 2
    };

    return render_pass_init(rp, &rp_info);
}

void render_pass_destroy(vk_render_pass* rp) {
    if (!rp->device || !rp->handle) {
        return;
    }
    vkDestroyRenderPass(rp->device, rp->handle, NULL);
    rp->handle = VK_NULL_HANDLE;
}
