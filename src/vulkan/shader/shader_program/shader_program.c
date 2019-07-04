#include "./shader_program.h"

#include "../../../memory/memory.h"
#include "../../../string/string.h"
#include "../../functions/functions.h"
#include "../shd.h"
#include "./render_state.h"

void shader_program_init_empty(shader_program* prog) {
    prog->device = VK_NULL_HANDLE;

    prog->pipeline_layout = VK_NULL_HANDLE;
    prog->descriptor_set_layout = VK_NULL_HANDLE;
    prog->vertex_layout = VERTEX_LAYOUT_UNKNOWN;

    for (size_t i = 0; i < PIPELINE_CACHE_SIZE; i++) {
        pipeline_state *p = &prog->pipeline_cache[i];
        p->state_bits = RST_DEFAULT;
        p->handle = VK_NULL_HANDLE;
        p->counter = 0;
    }

    for (size_t i = 0; i < SHADER_TYPES_COUNT; ++i) {
        mem_set(prog->shaders[i], 0, SHADER_MAX_NAME_SIZE);
    }
}

uint32_t shader_program_get_shader_type_bits(const shader_program* prog) {
    uint32_t result = 0;

    for (size_t i = 0; i < SHADER_TYPES_COUNT; ++i) {
        if (string_is_empty(prog->shaders[i])) {
            continue;
        }
        result += shader_type_index_map[i];
    }

    return result;
}

void shader_program_destroy(shader_program* prog) {
    if (prog->pipeline_layout) {
        vkDestroyPipelineLayout(prog->device, prog->pipeline_layout, NULL);
        prog->pipeline_layout = VK_NULL_HANDLE;
    }
    if (prog->descriptor_set_layout) {
        vkDestroyDescriptorSetLayout(prog->device, prog->descriptor_set_layout,
            NULL);
        prog->descriptor_set_layout = VK_NULL_HANDLE;
    }

    for (size_t i = 0; i < prog->pipeline_cache_size; i++) {
        pipeline_state *ps = &prog->pipeline_cache[i];
        if (ps->handle) {
            vkDestroyPipeline(prog->device, ps->handle, NULL);
            ps->handle = VK_NULL_HANDLE;
        }
    }

    for (size_t i = 0; i < SHADER_TYPES_COUNT; ++i) {
        mem_set(prog->shaders[i], 0, SHADER_MAX_NAME_SIZE);
    }
}
