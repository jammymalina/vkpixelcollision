#include "./main_renderer.h"

#include "../../logger/logger.h"
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
    renderer->active_render_res_idx = 0;
}

const rendering_resource* main_renderer_active_render_res(const main_renderer*
    renderer)
{
    uint32_t active_idx = renderer->active_render_res_idx;
    return &renderer->ctx->rendering_resources[active_idx];
}

static inline void main_renderer_set_viewport(const main_renderer* renderer) {
    const rendering_resource* render_res =
        main_renderer_active_render_res(renderer);

    VkViewport viewport = {
        x: 0,
        y: 0,
        width: renderer->ctx->width,
        height: renderer->ctx->height,
        minDepth: 0.0f,
        maxDepth: 1.0f
    };
    vkCmdSetViewport(render_res->command_buffer, 0, 1, &viewport);

    VkRect2D scissor = {
        offset: {
            x: 0,
            y: 0,
        },
        extent: {
            width: viewport.width,
            height: viewport.height
        }
    };
    vkCmdSetScissor(render_res->command_buffer, 0, 1, &scissor);

}

static bool main_renderer_prepare_frame(main_renderer* renderer) {
    const rendering_resource* render_res =
        main_renderer_active_render_res(renderer);

    VkCommandBufferBeginInfo command_buff_begin_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = NULL,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
      .pInheritanceInfo = NULL
    };
    CHECK_VK_BOOL(vkBeginCommandBuffer(render_res->command_buffer,
        &command_buff_begin_info));

    main_renderer_set_viewport(renderer);

    VkClearValue clear_values[2];
    uint32_t clear_values_size = 0;
    if (renderer->clear_bits & CLEAR_COLOR_BUFFER) {
        float clear_color[4];
        color_rgba_uint32_to_float(clear_color, renderer->clear_color);
        VkClearColorValue color;
        color.float32[0] = clear_color[0];
        color.float32[1] = clear_color[1];
        color.float32[2] = clear_color[2];
        color.float32[3] = clear_color[3];
        clear_values[clear_values_size++].color = color;
    }
    if (renderer->clear_bits & (CLEAR_DEPTH_BUFFER | CLEAR_STENCIL_BUFFER)) {
        VkClearDepthStencilValue depth_clear_value;
        depth_clear_value.depth = 1.0;
        depth_clear_value.stencil = 0;
        clear_values[clear_values_size++].depthStencil = depth_clear_value;
    }

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
        .clearValueCount = clear_values_size,
        .pClearValues = clear_values
    };
    vkCmdBeginRenderPass(render_res->command_buffer, &render_pass_begin_info,
        VK_SUBPASS_CONTENTS_INLINE);

    return true;
}

static inline bool main_renderer_sync(const main_renderer* renderer) {
    const rendering_resource* render_res =
        main_renderer_active_render_res(renderer);

    CHECK_VK_BOOL(vkWaitForFences(renderer->ctx->gpu->device, 1,
        &render_res->fence, VK_TRUE, UINT64_MAX));
    CHECK_VK_BOOL(vkResetFences(renderer->ctx->gpu->device, 1,
        &render_res->fence));

    return true;
}

static inline bool main_renderer_acquire_image(const main_renderer* renderer,
    uint32_t* image_index)
{
    const rendering_resource* render_res =
        main_renderer_active_render_res(renderer);

    VkResult result = vkAcquireNextImageKHR(renderer->ctx->gpu->device,
        renderer->ctx->swapchain.handle, UINT64_MAX,
        render_res->image_available_semaphore, VK_NULL_HANDLE, image_index);
    switch(result) {
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

    return true;
}

static bool main_renderer_finish_frame(main_renderer* renderer) {
    const rendering_resource* render_res =
        main_renderer_active_render_res(renderer);

    vkCmdEndRenderPass(render_res->command_buffer);
    CHECK_VK_BOOL(vkEndCommandBuffer(render_res->command_buffer));

    return true;
}

static inline bool main_renderer_present_sync(const main_renderer* renderer) {
    const rendering_resource* render_res =
        main_renderer_active_render_res(renderer);

    VkPipelineStageFlags wait_dst_stage_mask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &render_res->image_available_semaphore,
        .pWaitDstStageMask = &wait_dst_stage_mask,
        .commandBufferCount = 1,
        .pCommandBuffers = &render_res->command_buffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &render_res->finished_render_semaphore
    };
    CHECK_VK_BOOL(vkQueueSubmit(renderer->ctx->graphics_queue.handle, 1, &submit_info,
        render_res->fence));

    return true;
}

static inline bool main_renderer_present_frame(const main_renderer* renderer,
    const uint32_t* image_index)
{
    const rendering_resource* render_res =
        main_renderer_active_render_res(renderer);

    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &render_res->finished_render_semaphore,
        .swapchainCount = 1,
        .pSwapchains = &renderer->ctx->swapchain.handle,
        .pImageIndices = image_index,
        .pResults = NULL
    };
    VkResult result = vkQueuePresentKHR(renderer->ctx->graphics_queue.handle,
        &present_info);

    switch(result) {
      case VK_SUCCESS:
        break;
      case VK_ERROR_OUT_OF_DATE_KHR:
      case VK_SUBOPTIMAL_KHR:
        log_error("Problem occured during vkQueuePresentKHR - out of date or"
            " suboptimal");
        return false;
      default:
        log_error("Problem occured during vkQueuePresentKHR");
        return false;
    }

    return true;
}

static inline void main_renderer_next_rendering_res(main_renderer* renderer) {
    renderer->active_render_res_idx = (renderer->active_render_res_idx + 1) %
        renderer->ctx->swapchain.image_count;
}

bool main_renderer_render(main_renderer* renderer) {
    uint32_t image_index = 0;

    bool status = main_renderer_sync(renderer);
    ASSERT_LOG_ERROR(status, "Unable to reset fence");

    status &= main_renderer_acquire_image(renderer, &image_index);
    ASSERT_LOG_ERROR(status, "Unable to acquire image");

    status &= main_renderer_prepare_frame(renderer);
    ASSERT_LOG_ERROR(status, "Unable to prepare frame");

    status &= main_renderer_finish_frame(renderer);
    ASSERT_LOG_ERROR(status, "Unable to finish frame");

    status &= main_renderer_present_sync(renderer);
    ASSERT_LOG_ERROR(status, "Unable to present sync");

    status &= main_renderer_present_frame(renderer, &image_index);
    ASSERT_LOG_ERROR(status, "Unable to present an image");

    main_renderer_next_rendering_res(renderer);

    return true;
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
