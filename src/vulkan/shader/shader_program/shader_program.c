#include "./shader_program.h"

#include "../../../logger/logger.h"
#include "../../../memory/memory.h"
#include "../../../string/string.h"
#include "../../functions/functions.h"
#include "../../tools/tools.h"
#include "../shd.h"
#include "./render_state.h"

void shader_program_init_empty(shader_program* prog) {
    prog->device = VK_NULL_HANDLE;
    prog->render_pass = VK_NULL_HANDLE;
    prog->vk_pipeline_cache = VK_NULL_HANDLE;

    prog->pipeline_layout = VK_NULL_HANDLE;
    prog->descriptor_set_layout = VK_NULL_HANDLE;
    prog->vertex_layout = VERTEX_LAYOUT_UNKNOWN;

    for (size_t i = 0; i < SHADER_PROGRAM_PIPELINE_CACHE_SIZE; i++) {
        pipeline_state* p = &prog->pipeline_cache[i];
        p->state_bits = RST_DEFAULT;
        p->handle = VK_NULL_HANDLE;
        p->counter = 0;
    }

    for (size_t i = 0; i < SHADER_TYPES_COUNT; ++i) {
        prog->shaders[i] = NULL;
    }
}

bool shader_program_has_pipeline(const shader_program* prog, pipeline_state_bits
    state_bits)
{
    for (size_t i = 0; i < prog->pipeline_cache_size; i++) {
        pipeline_state *ps = &prog->pipeline_cache[i];
        if (ps->state_bits == state_bits) {
            return true;
        }
    }

    return false;
}

void shader_program_remove_least_used_pipeline_from_cache(shader_program*
    prog)
{
    size_t index = 0;
    size_t min_used = prog->pipeline_cache[index].counter;
    for (size_t i = 1; i < prog->pipeline_cache_size; i++) {
        if (prog->pipeline_cache[i].counter < min_used) {
            min_used = prog->pipeline_cache[i].counter;
            index = i;
        }
    }
    pipeline_state_destroy(&prog->pipeline_cache[index]);

    // shift left
    for (size_t i = index; i < prog->pipeline_cache_size - 1; ++i) {
        pipeline_state_copy(&prog->pipeline_cache[i],
            &prog->pipeline_cache[i + 1]);
    }

    // decrease size
    --prog->pipeline_cache_size;
}

void shader_program_add_pipeline_to_cache(shader_program* prog, const
    pipeline_state* ps)
{
    if (shader_program_has_pipeline(prog, ps->state_bits)) {
        return;
    }

    if (prog->pipeline_cache_size >= SHADER_PROGRAM_PIPELINE_CACHE_SIZE) {
        shader_program_remove_least_used_pipeline_from_cache(prog);
    }

    size_t cache_index = prog->pipeline_cache_size++;
    pipeline_state_copy(&prog->pipeline_cache[cache_index], ps);
}

const pipeline_state* shader_program_get_pipeline_by_state_bits(shader_program*
    prog, pipeline_state_bits state_bits)
{
    for (size_t i = 0; i < prog->pipeline_cache_size; i++) {
        pipeline_state *ps = &prog->pipeline_cache[i];
        if (ps->state_bits == state_bits) {
            ps->counter++;
            return ps;
        }
    }

    return NULL;
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
}
