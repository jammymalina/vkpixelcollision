#include "./shader_program_builder.h"

static inline void pshaders_sort_by_type(const shader* dest[SHADER_TYPES_COUNT],
    const shader** shaders, size_t shaders_size)
{
    for (size_t i = 0; i < SHADER_TYPES_COUNT; ++i) {
        dest[i] = NULL;
    }

    for (size_t i = 0; i < shaders_size; ++i) {
        size_t shader_index = shader_type_to_index(shaders[i]->type);
        if (dest[shader_index] != NULL) {
            log_warning("Render program: shader type duplicity");
        }
        dest[shader_index] = shaders[i];
    }
}
