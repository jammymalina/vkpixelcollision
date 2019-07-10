#include "./shader_program_builder.h"

#include "../../../logger/logger.h"
#include "../../functions/functions.h"
#include "../../tools/tools.h"
#include "../shd.h"

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

static bool create_pipeline_layout(shader_program* prog) {
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

bool shader_builder_build_shader_program(shader_program* prog, const
    shader_program_create_info* prog_info)
{
    shader_program_init_empty(prog);
    prog->gpu = prog_info->gpu;
    ASSERT_LOG_ERROR(prog->gpu, "GPU must be initialized when building shader");

    shaders_sort_by_type(prog->shaders, prog_info->shaders,
        prog_info->shaders_size);

    bool success = create_descriptor_layout(prog);
    ASSERT_LOG_ERROR(success, "Unable to create descriptor layout");
    success = create_pipeline_layout(prog);
    ASSERT_LOG_ERROR(success, "Unable to create pipeline layout");

    for (size_t i = 0; i < prog_info->preconfigured_pipelines_size; ++i) {
        pipeline_state tmp;
        const pipeline_create_info pipe_info = {
            .state_bits = prog_info->preconfigured_pipelines[i],
            .store_in_program_cache = true
        };
        success = pipeline_builder_build_pipeline(&tmp, prog, &pipe_info);
        ASSERT_LOG_ERROR(success, "Unable to create pipeline");
    }

    return true;
}
