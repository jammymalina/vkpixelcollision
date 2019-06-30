#include "./shader_manager.h"

#include "../../../logger/logger.h"
#include "../loader/shader_loader.h"

void shader_manager_init_empty(shader_manager* shm) {
    hash_string_map_init(&shm->shaders);
}

bool shader_manager_init(shader_manager* shm, const shader_manager_create_info*
    shm_info)
{
    shader_manager_init_empty(shm);
    size_t expected_capacity = shm_info->expected_number_of_shaders == 0 ?
        2 * DEFAULT_EXPECTED_NUMBER_OF_SHADERS :
        2 * shm_info->expected_number_of_shaders;
    bool status = hash_string_map_reserve(&shm->shaders, expected_capacity);
    if (!status) {
        log_error("Unable to reserve space for shader manager map");
        return false;
    }
    return true;
}

bool shader_manager_add(shader_manager* shm, const char* shader_name, shader
    program)
{
    if (shader_manager_has(shm, shader_name)) {
        log_warning("Overwriting shader in the shader manager, shader resource"
            " was not destroyed");
    }
    return hash_string_map_add(&shm->shaders, shader_name, program);
}

bool shader_manager_get(shader_manager* shm, const char* shader_name, shader*
    program)
{
    return hash_string_map_get(&shm->shaders, shader_name, program);
}

bool shader_manager_has(shader_manager* shm, const char* shader_name) {
    return hash_string_map_has(&shm->shaders, shader_name);
}

bool shader_manager_delete(shader_manager* shm, const char* shader_name) {
    return hash_string_map_delete(&shm->shaders, shader_name);
}

bool shader_manager_preload(shader_manager* shm, const shader_preload_info*
    preload_info, const char basepath[PATH_MAX_SIZE], const gpu_info* gpu)
{
    bool status = true;
    for (size_t i = 0; i < preload_info->preloaded_shaders_size; ++i) {
        shader s;
        char shd_path[PATH_MAX_SIZE];
        const char* shd_rel_path = preload_info->preloaded_shaders[i].filepath;
        const char* shd_name = preload_info->preloaded_shaders[i].name;

        status &= path_append_to_basepath(shd_path, basepath, shd_rel_path) &&
            shader_loader_load_shader(&s, shd_path, shd_name, gpu);
        if (!status) {
            log_error("Unable to create shader %s: %s", shd_name, shd_path);
            return false;
        }
        log_info("Loaded shader %s: %s", shd_name, shd_path);
    }
    return status;
}

void shader_manager_destroy(shader_manager* shm) {
    const size_t shader_buff_size = 32;
    size_t shaders_processed = 0;
    shader shader_buff[shader_buff_size];
    while (shaders_processed < hash_string_map_get_size(&shm->shaders)) {
        const size_t current_processed = hash_string_map_values_range(
            &shm->shaders, shader_buff, shaders_processed, shader_buff_size);
        shaders_processed += current_processed;
        for (size_t i = 0; i < current_processed; ++i) {
            shader_destroy(&shader_buff[i]);
        }
    }
    hash_string_map_clear(&shm->shaders);
}
