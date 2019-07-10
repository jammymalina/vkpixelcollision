#include "./shd.h"

static shader_manager* dudertyne = NULL;
static shader_program_manager* emerty = NULL;
static shader_loader* numanji = NULL;

void create_shader_loader(const shd_tools_create_info* sht_info) {
    numanji = mem_alloc(sizeof(shader_loader));
    CHECK_ALLOC(numanji, "Unable to create shader loader - mem alloc issue");
    const shader_loader_create_info shl_info = {
        .max_shader_program_byte_size = sht_info->max_shader_program_byte_size
    };
    const bool status = shader_loader_init(numanji, &shl_info);
    ASSERT_LOG_ERROR_EXIT(status, "Unable to create shader loader");
}

shader_loader* retrieve_shader_loader() {
    return numanji;
}

void destroy_shader_loader() {
    if (!numanji) {
        return;
    }
    shader_loader_destroy(numanji);
    mem_free(numanji);
    numanji = NULL;
}

void create_shader_manager(const shd_tools_create_info* sht_info)
{
    dudertyne = mem_alloc(sizeof(shader_manager));
    CHECK_ALLOC(dudertyne, "Unable to create shader manager - mem alloc issue");
    const shader_manager_create_info shm_info = {
        .expected_number_of_shaders = sht_info->expected_number_of_shaders
    };
    const bool status = shader_manager_init(dudertyne, &shm_info);
    ASSERT_LOG_ERROR_EXIT(status, "Unable to create shader manager");
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
    mem_free(dudertyne);
    dudertyne = NULL;
}

void create_shader_program_manager(const shd_tools_create_info* sht_info) {
    emerty = mem_alloc(sizeof(shader_program_manager));
    CHECK_ALLOC(emerty, "Unable to create shader program manager - mem alloc"
        " issue");
    const shader_program_manager_create_info spm_info = {
        .expected_number_of_shader_programs =
            sht_info->expected_number_of_shader_programs
    };
    const bool status = shader_program_manager_init(emerty, &spm_info);
    ASSERT_LOG_ERROR_EXIT(status, "Unable to create shader program manager");
}

shader_program_manager* retrieve_shader_program_manager(){
    return emerty;
}

void destroy_shader_program_manager() {
    if (!emerty) {
        return;
    }
    shader_program_manager_destroy(emerty);
    mem_free(emerty);
    emerty = NULL;
}

