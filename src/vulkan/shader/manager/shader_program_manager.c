#include "./shader_program_manager.h"

#include "../shd.h"

void shader_program_manager_init_empty(shader_program_manager* spm) {
    hash_string_map_init(&spm->programs);
}

bool shader_program_manager_init(shader_program_manager* spm, const
    shader_program_manager_create_info* spm_info)
{
    shader_program_manager_init_empty(spm);

    size_t expected_capacity = 2 * spm_info->expected_number_of_shader_programs;
    expected_capacity = expected_capacity == 0 ?
        2 * DEFAULT_EXPECTED_NUMBER_OF_SHADER_PROGRAMS : expected_capacity;
    bool status = hash_string_map_reserve(&spm->programs, expected_capacity);
    ASSERT_LOG_ERROR(status, "Unable to reserve space for shader program"
        " manager map");
    return true;
}

bool shader_program_manager_add(shader_program_manager* spm, const char*
    shader_name, const shader_program* program)
{
    if (shader_program_manager_has(spm, shader_name)) {
        log_warning("Overwriting shader program in the shader program manager,"
            " shader resource was not destroyed");
    }
    shader_program placeholder;
    shader_program_init_empty(&placeholder);
    bool status = hash_string_map_add(&spm->programs, shader_name, placeholder);
    ASSERT_LOG_ERROR(status, "Unable to add shader program: %s", shader_name);

    shader_program* p = shader_program_manager_get_reference(spm, shader_name);
    ASSERT_LOG_ERROR(p, "Unable to retrieve recently added shader program: %s",
        shader_name);
    shader_program_copy(p, program);
    return true;
}

bool shader_program_manager_get(shader_program_manager* spm, const char*
    shader_name, shader_program* program)
{
    return hash_string_map_get(&spm->programs, shader_name, program);
}

shader_program* shader_program_manager_get_reference(const
    shader_program_manager* spm, const char* shader_name)
{
    return hash_string_map_get_reference(&spm->programs, shader_name);
}

bool shader_program_manager_has(shader_program_manager* spm, const char*
    shader_name)
{
    return hash_string_map_has(&spm->programs, shader_name);
}

bool shader_program_manager_delete(shader_program_manager* spm, const char*
    shader_name)
{
    shader_program* p = shader_program_manager_get_reference(spm, shader_name);
    if (p) {
        shader_program_destroy(p);
    }
    return hash_string_map_delete(&spm->programs, shader_name);
}

bool shader_program_manager_preload(shader_program_manager* spm, const
    shader_program_preload_info* preload_info)
{
    bool status = true;

    for (size_t i = 0; i < preload_info->shader_programs_config_size; ++i) {
        shader_program p;
        shader_manager_program_create_info prog_info =
            preload_info->shader_programs_config[i];
        prog_info.gpu = prog_info.gpu ?
            prog_info.gpu : preload_info->default_gpu;
        prog_info.default_render_pass = prog_info.default_render_pass ?
            prog_info.default_render_pass : preload_info->default_render_pass;

        status &= shader_builder_preload_programs(&p, &prog_info);
        ASSERT_LOG_ERROR(status, "Unable to build shader program: %s",
            prog_info.name);

        log_info("Built shader program: %s", prog_info.name);
        status &= shader_program_manager_add(spm, prog_info.name, &p);
        ASSERT_LOG_ERROR(status, "Unable to add shader to shader manager: %s",
            prog_info.name);
    }

    return status;
}

void shader_program_manager_destroy(shader_program_manager* spm) {
    const size_t shader_buff_size = 32;
    size_t shaders_processed = 0;
    shader_program* shader_buff[shader_buff_size];
    while (shaders_processed < hash_string_map_get_size(&spm->programs)) {
        const size_t current_processed = hash_string_map_values_reference_range(
            &spm->programs, shader_buff, shaders_processed, shader_buff_size);
        shaders_processed += current_processed;
        for (size_t i = 0; i < current_processed; ++i) {
            shader_program_destroy(shader_buff[i]);
        }
    }
    hash_string_map_clear(&spm->programs);
}
