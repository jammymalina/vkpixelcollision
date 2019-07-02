#include "./shader_loader.h"

#include "../../../file/file.h"
#include "../../../file/path.h"
#include "../../../logger/logger.h"
#include "../../../memory/memory.h"
#include "../shader_program/shader.h"

void shader_loader_init_empty(shader_loader* shl) {
    shl->max_shader_program_byte_size = 0;
    shl->program_buffer = NULL;
}

bool shader_loader_init(shader_loader* shl, const shader_loader_create_info*
    shl_config)
{
    shader_loader_init_empty(shl);
    const size_t max_byte_size = shl_config->max_shader_program_byte_size == 0 ?
            DEFAULT_MAX_SHADER_PROGRAM_BYTE_SIZE :
            shl_config->max_shader_program_byte_size;
    shl->program_buffer = mem_alloc(max_byte_size);
    CHECK_ALLOC_BOOL(shl->program_buffer, "Unable to allocate buffer for shader"
        " loader");

    if (!is_4_byte_aligned(shl->program_buffer)) {
        log_error("Shader code bytes are not aligned to 4 bytes");
        return false;
    }

    shl->max_shader_program_byte_size = max_byte_size;

    return true;
}

bool shader_loader_load_shader(const shader_loader* shl, shader* shd, const
    char* filename, const char name[SHADER_MAX_NAME_SIZE], const gpu_info* gpu)
{
    CHECK_ALLOC_BOOL(shl, "Allocate shader loader first before shader load op");
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
    if (shader_file_size > shl->max_shader_program_byte_size) {
        log_error("Shader binary file is too large");
        return false;
    }

    ssize_t total_bytes_read = file_read_binary(filename, shl->program_buffer);

    if (total_bytes_read < 0) {
        log_error("Unable to read shader");
        return false;
    }

    shader_create_info shader_info = {
        .type = shd_type,
        .device = gpu->device,
        .program = (uint32_t*) shl->program_buffer,
        .program_size = shader_file_size,
    };
    bool shader_status = shader_init(shd, &shader_info);

    return shader_status;
}

void shader_loader_destroy(shader_loader* shl) {
    if (shl->program_buffer) {
        mem_free(shl->program_buffer);
        shl->max_shader_program_byte_size = 0;
    }
}
