#include "./shd.h"

static shader_manager* dudertyne = NULL;

void create_shader_manager(const shader_manager_create_info* shm_info)
{
    dudertyne = mem_alloc(sizeof(shader_manager));
    CHECK_ALLOC(dudertyne, "Unable to shader manager - mem alloc issue");
    if (!shader_manager_init(dudertyne, shm_info)) {
        log_error("Unable to create shader manager");
        exit(EXIT_FAILURE);
    }
}

shader_manager* retrieve_shader_manager()
{
    return dudertyne;
}

void destroy_shader_manager() {
    if (!dudertyne) {
        return;
    }
    shader_manager_destroy(dudertyne);
    dudertyne = NULL;
}

