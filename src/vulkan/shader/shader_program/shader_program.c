#include "./shader_program.h"

#include "../../../logger/logger.h"
#include "../../../memory/memory.h"
#include "../../../string/string.h"
#include "../../functions/functions.h"
#include "../../tools/tools.h"
#include "../shd.h"
#include "./render_state.h"

void shader_program_init_empty(shader_program* prog) {
    prog->gpu = NULL;
    prog->vk_pipeline_cache = VK_NULL_HANDLE;

    prog->pipeline_layout = VK_NULL_HANDLE;
    prog->descriptor_set_layout = VK_NULL_HANDLE;
    prog->vertex_layout = VERTEX_LAYOUT_UNKNOWN;

    prog->pipeline_cache_size = 0;
    for (size_t i = 0; i < SHADER_PROGRAM_PIPELINE_CACHE_SIZE; i++) {
        pipeline_state* p = &prog->pipeline_cache[i];
        pipeline_state_init_empty(p);
    }

    for (size_t i = 0; i < SHADER_TYPES_COUNT; ++i) {
        prog->shaders[i] = NULL;
    }
}

// DANGEROUS COPY!!! USE WITH CAUTION
void shader_program_copy(shader_program* dest, const shader_program* src) {
    dest->gpu = src->gpu;
    dest->vk_pipeline_cache = src->vk_pipeline_cache;
    mem_copy(dest->shaders, src->shaders, sizeof(shader*) * SHADER_TYPES_COUNT);
    dest->pipeline_layout = src->pipeline_layout;
    dest->descriptor_set_layout = src->descriptor_set_layout;
    dest->vertex_layout = src->vertex_layout;
    mem_copy(dest->pipeline_cache, src->pipeline_cache,
        SHADER_PROGRAM_PIPELINE_CACHE_SIZE * sizeof(pipeline_state));
    dest->pipeline_cache_size = src->pipeline_cache_size;
}

bool shader_program_has_pipeline(const shader_program* prog, pipeline_state_bits
    state_bits)
{
    for (size_t i = 0; i < prog->pipeline_cache_size; i++) {
        const pipeline_state *ps = &prog->pipeline_cache[i];
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
    if (prog->pipeline_cache[cache_index].gpu != prog->gpu) {
        log_warning("Render program's GPU is different than pipeline GPU");
    }
}

const pipeline_state* shader_program_get_pipeline(shader_program*
    prog, pipeline_state_bits state_bits, VkRenderPass render_pass)
{
    for (size_t i = 0; i < prog->pipeline_cache_size; ++i) {
        pipeline_state *ps = &prog->pipeline_cache[i];
        if (ps->state_bits == state_bits && ps->render_pass == render_pass) {
            ps->counter++;
            return ps;
        }
    }

    return NULL;
}

void shader_program_destroy(shader_program* prog) {
    log_info("Destroying shader program");
    if (prog->pipeline_layout && prog->gpu) {
        vkDestroyPipelineLayout(prog->gpu->device, prog->pipeline_layout, NULL);
        prog->pipeline_layout = VK_NULL_HANDLE;
    }
    if (prog->descriptor_set_layout && prog->gpu) {
        vkDestroyDescriptorSetLayout(prog->gpu->device, prog->descriptor_set_layout,
            NULL);
        prog->descriptor_set_layout = VK_NULL_HANDLE;
    }

    for (size_t i = 0; i < prog->pipeline_cache_size; ++i) {
        pipeline_state *ps = &prog->pipeline_cache[i];
        pipeline_state_destroy(ps);
    }
}
