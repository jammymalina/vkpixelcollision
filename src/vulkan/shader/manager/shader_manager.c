#include "./shader_manager.h"

#include "../../../logger/logger.h"

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

void shader_manager_destroy(shader_manager* shm) {
}
