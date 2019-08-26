#include "./main_renderer.h"

#include "../../material/color.h"
#include "../../memory/memory.h"
#include "../../vulkan/functions/functions.h"
#include "../../vulkan/tools/tools.h"

static main_renderer* comeraano = NULL;

void main_renderer_init(main_renderer* renderer, const
    main_renderer_create_info* renderer_info)
{
    renderer->ctx = renderer_info->ctx;
    renderer->clear_color = renderer_info->clear_color;
    renderer->clear_bits = renderer_info->clear_bits;
}

const rendering_resource* main_renderer_active_render_res(const main_renderer*
    renderer)
{
    uint32_t active_idx = renderer->active_recording_frame;
    return &renderer->ctx->rendering_resources[active_idx];
}

static bool main_renderer_prepare_frame(main_renderer* renderer) {
    const rendering_resource* render_res =
        main_renderer_get_active_command_buffer(renderer);

    VkCommandBufferBeginInfo command_buff_begin_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = NULL,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
      .pInheritanceInfo = NULL
    };
    CHECK_VK_BOOL(vkBeginCommandBuffer(render_res->command_buffer,
        &command_buff_begin_info));

    VkRenderPassBeginInfo render_pass_begin_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = NULL,
        .renderPass = renderer->ctx->render_pass.handle,
        .framebuffer = render_res->framebuffer.handle,
        .renderArea = {
            .offset = {
                .x = 0,
                .y = 0
            },
            .extent = renderer->ctx->swapchain.extent
        },
        .clearValueCount = 0,
        .pClearValues    = NULL
    };
    vkCmdBeginRenderPass(render_res->command_buffer, &render_pass_begin_info,
        VK_SUBPASS_CONTENTS_INLINE);

    main_renderer_clear_screen(renderer, renderer->clear_color,
        renderer->clear_bits);

    return true;
}

static inline bool main_renderer_sync(const main_renderer* renderer) {
    const rendering_resource* render_res =
        main_renderer_get_active_command_buffer(renderer);

    CHECK_VK_BOOL((renderer->ctx->gpu->device, 1, render_res->fence, VK_TRUE,
        UINT64_MAX));
    CHECK_VK_BOOL(vkResetFences(renderer->ctx->gpu->device, 1,
        render_res->fence));

    return true;
}

static inline bool main_renderer_acquire_image(const main_renderer* renderer,
    uint32_t* image_index)
{
    const rendering_resource* render_res =
        main_renderer_get_active_command_buffer(renderer);

    VkResult result = vkAcquireNextImageKHR(renderer->ctx->gpu->device,
        renderer->ctx->swapchain.handle, UINT64_MAX,
        render_res->image_available_semaphore, VK_NULL_HANDLE, image_index);
    switch( result ) {
      case VK_SUCCESS:
      case VK_SUBOPTIMAL_KHR:
        break;
      case VK_ERROR_OUT_OF_DATE_KHR:
        log_error("Resize occured - refresh the swapchain");
        return false;
      default:
        log_error("Problem occurred during swap chain image acquisition!");
        return false;
    }
}

static bool main_renderer_finish_frame(main_renderer* renderer) {
    const rendering_resource* render_res =
        main_renderer_get_active_command_buffer(renderer);

    vkCmdEndRenderPass(render_res->command_buffer);
    CHECK_VK_BOOL(vkEndCommandBuffer(render_res->command_buffer));

    return true;
}

bool main_renderer_render(main_renderer* renderer) {
    uint32_t image_index = 0;

    bool status = main_renderer_sync(renderer);
    ASSERT_LOG_ERROR("Unable to reset fence");

    status &= main_renderer_acquire_image(renderer, &image_index);
    ASSERT_LOG_ERROR(status, "Unable to acquire image");

    status &= main_renderer_prepare_frame(renderer);
    ASSERT_LOG_ERROR(status, "Unable to prepare frame");

    status &= main_renderer_finish_frame(renderer);
    ASSERT_LOG_ERROR(status, "Unable to finish frame");

    return true;
}

void main_renderer_clear_screen(main_renderer* renderer, uint32_t color,
    uint32_t clear_bits)
{
    uint32_t num_attachments = 0;
    VkClearAttachment attachments[2] = {
        { 0 },
        { 0 }
    };

    float rgba[4] = { 0, 0, 0, 0 };
    color_rgb_uint32_to_float(rgba, renderer->clear_color);
    uint32_t stencil_value = 0;

    if (clear_bits & CLEAR_COLOR_BUFFER) {
        VkClearAttachment *attachment = &attachments[num_attachments++];
        attachment->aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        attachment->colorAttachment = 0;
        VkClearColorValue *color = &attachment->clearValue.color;
        color->float32[0] = rgba[0];
        color->float32[1] = rgba[1];
        color->float32[2] = rgba[2];
        color->float32[3] = rgba[3];
    }

    if (clear_bits & (CLEAR_DEPTH_BUFFER | CLEAR_STENCIL_BUFFER)) {
        VkClearAttachment *attachment = &attachments[num_attachments++];
        if (clear_bits & CLEAR_DEPTH_BUFFER) {
            attachment->aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        if (clear_bits & CLEAR_STENCIL_BUFFER) {
            attachment->aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
        attachment->clearValue.depthStencil.depth = 1.0;
        attachment->clearValue.depthStencil.stencil = stencil_value;
    }

    VkClearRect clear_rect = {
        .rect = {
            .offset = {
                .x = 0,
                .y = 0
            },
            .extent = renderer->ctx->swapchain.extent
        },
        .baseArrayLayer = 0,
        .layerCount = 1
    };

    const rendering_resource* render_res =
        main_renderer_get_active_command_buffer(renderer);
    vkCmdClearAttachments(render_res->command_buffer, num_attachments,
        attachments, 1, &clear_rect);
}

void create_main_renderer(const main_renderer_create_info* renderer_info) {
    comeraano = mem_alloc(sizeof(main_renderer));
    CHECK_ALLOC(comeraano, "Unable to allocate main renderer");
    main_renderer_init(comeraano, renderer_info);
}

main_renderer* retrieve_main_renderer() {
    return comeraano;
}

void destroy_main_renderer() {
    mem_free(comeraano);
    comeraano = NULL;
}
