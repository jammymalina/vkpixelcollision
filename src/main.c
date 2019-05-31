#include "./app_window/window.h"

#include <unistd.h>
#include "./logger/logger.h"
#include "./vulkan/memory/allocator/vma.h"
#include "./vulkan/memory/allocator/lib/vma_vector.h"

int main(int argc, char* args[]) {
    app_window_config window_config = {
        .title = "Collision app",
        .width = 800,
        .height = 600
    };
    app_window window = create_window(&window_config);
    rendering_context ctx = get_rendering_context(&window);

    log_info("Window size: %d %d", window.width, window.height);
    log_info("Rendering context size: %d %d", ctx.width, ctx.height);
    log_info("Screen BPP: %d", SDL_BITSPERPIXEL(window.mode.format));

    vma_allocator* allocator = retrieve_vma_allocator(ctx.gpu.physical_device,
        ctx.gpu.device);
    vma_allocator* allocator2 = retrieve_vma_allocator(ctx.gpu.physical_device,
        ctx.gpu.device);

    sleep(5);

    destroy_rendering_context(&ctx);
    destroy_window(&window);
}
