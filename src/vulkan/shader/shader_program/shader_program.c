#include "./shader_program.h"

#include "../../../logger/logger.h"
#include "../../../memory/memory.h"
#include "../../../string/string.h"
#include "../../functions/functions.h"
#include "../../tools/tools.h"
#include "../shd.h"
#include "./render_state.h"

static inline void shaders_sort_by_type(const shader* dest[SHADER_TYPES_COUNT],
    const char** shaders, size_t shaders_size)
{
    shader_manager* shm = retrieve_shader_manager();

    for (size_t i = 0; i < SHADER_TYPES_COUNT; ++i) {
        dest[i] = NULL;
    }

    for (size_t i = 0; i < shaders_size; ++i) {
        const char* shader_name = shaders[i];
        if (!shader_manager_has(shm, shader_name)) {
            continue;
        }

        const shader* s = shader_manager_get_reference(shm, shader_name);
        if (!s) {
            continue;
        }

        size_t shader_index = shader_type_to_index(s->type);
        if (dest[shader_index] != NULL) {
            log_warning("Render program: shader type duplicity");
        }
        dest[shader_index] = s;
    }
}

static bool create_descriptor_layout(shader_program* prog)
{
    VkDescriptorSetLayoutBinding
        layout_bindings[SHADER_TYPES_COUNT * SHADER_MAX_BINDINGS_SIZE];

    uint32_t bindings_count = 0;

    for (size_t i = 0; i < SHADER_TYPES_COUNT; ++i) {
        const shader* s = prog->shaders[i];

        if (!s) {
            continue;
        }

        for (size_t bind_iter = 0; bind_iter < s->bindings_size; ++bind_iter) {
            VkDescriptorSetLayoutBinding binding = {
                .binding = bindings_count,
                .descriptorType =
                    shader_binding_to_descriptor_type(s->bindings[bind_iter]),
                .descriptorCount = 1,
                .stageFlags = shader_type_to_stage(s->type),
                .pImmutableSamplers = NULL
            };
            layout_bindings[bindings_count++] = binding;
        }
    }

    VkDescriptorSetLayoutCreateInfo descriptor_set_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .bindingCount = bindings_count,
        .pBindings = bindings_count == 0 ? NULL : layout_bindings
    };

    CHECK_VK_BOOL(vkCreateDescriptorSetLayout(prog->gpu->device,
        &descriptor_set_info, NULL, &prog->descriptor_set_layout));

    return true;
}

static inline bool create_pipeline_layout(shader_program* prog) {
    VkPipelineLayoutCreateInfo pipeline_layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .setLayoutCount = 1,
        .pSetLayouts = &prog->descriptor_set_layout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL
    };
    CHECK_VK_BOOL(vkCreatePipelineLayout(prog->gpu->device,
        &pipeline_layout_info, NULL, &prog->pipeline_layout));

    return true;
}

static inline bool create_pipeline_cache(shader_program* prog) {
    VkPipelineCacheCreateInfo pipeline_cache_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .initialDataSize = 0,
        .pInitialData = NULL
    };

    CHECK_VK_BOOL(vkCreatePipelineCache(prog->gpu->device, &pipeline_cache_info,
        NULL, &prog->vk_pipeline_cache));

    return true;
}

void shader_program_init_empty(shader_program* prog) {
    prog->type = SHADER_PROGRAM_TYPE_UNDEFINED;

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

bool shader_program_init(shader_program* prog, const shader_program_create_info*
    prog_info)
{
    shader_program_init_empty(prog);
    prog->type = prog_info->type;
    ASSERT_LOG_ERROR(prog->type != SHADER_PROGRAM_TYPE_UNDEFINED, "Shader"
        " program type cannot be undefined");

    prog->gpu = prog_info->gpu;
    ASSERT_LOG_ERROR(prog->gpu, "GPU must be initialized when building shader");
    prog->vertex_layout = prog_info->vertex_layout;
    bool success = prog->vertex_layout != VERTEX_LAYOUT_UNKNOWN &&
        prog->vertex_layout != VERTEX_LAYOUTS_TOTAL;
    ASSERT_LOG_ERROR(prog->vertex_layout, "Vertex layout cannot be unknown or"
        " total");

    shaders_sort_by_type(prog->shaders, prog_info->shaders,
        prog_info->shaders_size);

    success = create_descriptor_layout(prog);
    ASSERT_LOG_ERROR(success, "Unable to create descriptor layout");
    success = create_pipeline_layout(prog);
    ASSERT_LOG_ERROR(success, "Unable to create pipeline layout");
    success = create_pipeline_cache(prog);
    ASSERT_LOG_ERROR(success, "Unable to create pipeline cache");

    return true;
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

bool shader_program_bind_pipeline(const shader_program* prog,
    pipeline_state_bits state_bits, VkRenderPass render_pass, VkCommandBuffer
    command_buffer)
{
    ASSERT_LOG_WARNING(shader_program_has_pipeline(prog, state_bits,
        render_pass), "Unable to bind the desired pipeline");
    const pipeline_state* ps = shader_program_get_pipeline(prog, state_bits,
        render_pass);
    ASSERT_LOG_ERROR(ps, "Unable to bind pipeline, pipeline state is NULL");

    VkPipelineBindPoint bind_point = prog->type == SHADER_PROGRAM_TYPE_COMPUTE ?
        VK_PIPELINE_BIND_POINT_COMPUTE : VK_PIPELINE_BIND_POINT_GRAPHICS;
    vkCmdBindPipeline(command_buffer, bind_point, ps->handle);

    return true;
}

bool shader_program_has_pipeline(const shader_program* prog, pipeline_state_bits
    state_bits, VkRenderPass render_pass)
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
    if (shader_program_has_pipeline(prog, ps->state_bits, ps->render_pass)) {
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

    if (prog->vk_pipeline_cache && prog->gpu) {
        vkDestroyPipelineCache(prog->gpu->device, prog->vk_pipeline_cache,
            NULL);
    }
}
