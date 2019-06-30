#include "./shader_loader.h"

#include "../../../file/file.h"
#include "../../../file/path.h"
#include "../../../logger/logger.h"
#include "../../../memory/memory.h"
#include "../shader_program/shader.h"

bool shader_loader_load_shader(shader* shd, const char* filename,
    const char name[SHADER_MAX_NAME_SIZE], const gpu_info* gpu)
{
    shader_init_empty(shd);

    char shd_ext[PATH_MAX_EXTENSION_SIZE];
    bool ext_status = path_extract_extension_nth(filename, shd_ext, 2);
    if (!ext_status) {
        log_error("Unable to extract shader extension");
        return false;
    }
    shader_type shd_type = shader_extension_to_type(shd_ext);
    if (shd_type == SHADER_TYPE_UNDEFINED) {
        log_error("Unknown shader extension");
        return false;
    }

    ssize_t shader_file_size = file_retrieve_byte_size(filename);
    if (shader_file_size <= 0) {
        log_error("Unable to read shader file: %s", filename);
        return false;
    }

    char* shader_bytes = NULL;
    ssize_t total_bytes_read = file_read_binary(filename, &shader_bytes);

    if (total_bytes_read < 0) {
        log_error("Unable to read shader");
        return false;
    }

    if (!is_4_byte_aligned(shader_bytes)) {
        log_error("Shader code bytes are not aligned to 4 bytes");
        mem_free(shader_bytes);
        return false;
    }

    shader_create_info shader_info = {
        .type = shd_type,
        .device = gpu->device,
        .program = (uint32_t*) shader_bytes,
        .program_size = shader_file_size,
    };
    bool shader_status = shader_init(shd, &shader_info);

    mem_free(shader_bytes);

    return shader_status;
}
