#include "./pipeline_builder.h"

#include "../../../logger/logger.h"
#include "../../functions/functions.h"
#include"../../tools/tools.h"
#include "../../vertex/core/vertex_layout.h"

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

    CHECK_VK_BOOL(vkCreateDescriptorSetLayout(prog->gpu->device, &descriptor_set_info,
        NULL, &prog->descriptor_set_layout));

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
    CHECK_VK_BOOL(vkCreatePipelineLayout(prog->gpu->device, &pipeline_layout_info,
        NULL, &prog->pipeline_layout));

    return true;
}

static bool create_pipeline(VkPipeline *pipeline, pipeline_state_bits
    state_bits, shader_program* prog)
{
    vertex_layout vertex_layouts[VERTEX_LAYOUTS_TOTAL];
    vertex_layout* layout = &vertex_layouts[prog->vertex_layout];

    // vertex input
    VkPipelineVertexInputStateCreateInfo vertex_input_state =
        layout->input_state;
    vertex_input_state.vertexBindingDescriptionCount =
        layout->binding_desc_size;
	vertex_input_state.pVertexBindingDescriptions = layout->binding_desc;
	vertex_input_state.vertexAttributeDescriptionCount =
        layout->attribute_desc_size;
	vertex_input_state.pVertexAttributeDescriptions = layout->attribute_desc;

    // input assembly
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };

    // rasterization
    pipeline_state ps = { .state_bits = state_bits };
    VkPipelineRasterizationStateCreateInfo rasterization_state =
        pipeline_state_get_rasterization_state(&ps);

    // color blend attachment
    VkPipelineColorBlendAttachmentState color_blend_attachment =
        pipeline_state_get_color_blend_attachment(&ps);

    // color blend
    VkPipelineColorBlendStateCreateInfo color_blend = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext= NULL,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_CLEAR,
        .attachmentCount = 1,
        .pAttachments = &color_blend_attachment,
        .blendConstants = { 0, 0, 0, 0 }
    };

    // depth / stencil
    VkPipelineDepthStencilStateCreateInfo depth_stencil_info =
        pipeline_state_get_depth_stencil_info(&ps);

    // multisample
    VkPipelineMultisampleStateCreateInfo multisample_info =
        pipeline_state_get_multisample_info(&ps);

    VkPipelineShaderStageCreateInfo shader_stages[SHADER_TYPES_COUNT];
    size_t shader_stages_size = 0;

    for (size_t i = 0; i < SHADER_TYPES_COUNT; i++) {
        if (!prog->shaders[i])
            continue;

        const shader* s = prog->shaders[i];

        VkPipelineShaderStageCreateInfo shader_stage_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .stage = shader_type_to_stage(s->type),
            .module = s->module,
            .pName = "main",
            .pSpecializationInfo = NULL
        };
        shader_stages[shader_stages_size] = shader_stage_info;

        shader_stages_size++;
    }

    // dynamic state
    VkDynamicState dynamic_states[8];
    size_t dynamic_states_size = 0;
    pipeline_state_get_dynamic_states(&ps, NULL, &dynamic_states_size);
    if (dynamic_states_size > 8) {
        log_error("Render program: not enough progace for dynamic states");
        return false;
    }
    pipeline_state_get_dynamic_states(&ps, dynamic_states, &dynamic_states_size);

    VkPipelineDynamicStateCreateInfo dynamic_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .dynamicStateCount = dynamic_states_size,
        .pDynamicStates = dynamic_states
    };

    // pViewports and pScissors ignored because of dynamic state
    VkPipelineViewportStateCreateInfo viewport_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .viewportCount = 1,
        .pViewports = NULL,
        .scissorCount = 1,
        .pScissors = NULL
    };

    // pipeline
    VkGraphicsPipelineCreateInfo pipeline_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stageCount = shader_stages_size,
        .pStages = shader_stages,
        .pVertexInputState = &vertex_input_state,
        .pInputAssemblyState = &input_assembly,
        .pTessellationState = NULL,
        .pViewportState = &viewport_info,
        .pRasterizationState = &rasterization_state,
        .pMultisampleState = &multisample_info,
        .pDepthStencilState = &depth_stencil_info,
        .pColorBlendState = &color_blend,
        .pDynamicState = &dynamic_info,
        .layout = prog->pipeline_layout,
        .renderPass = prog->render_pass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0
    };

    *pipeline = VK_NULL_HANDLE;
    CHECK_VK_BOOL(vkCreateGraphicsPipelines(prog->gpu->device,
        prog->vk_pipeline_cache, 1, &pipeline_info, NULL, pipeline));

    return true;
}

bool pipeline_builder_build_pipeline(pipeline_state* result,
    shader_program* prog, const pipeline_create_info* pipe_info)
{
    const pipeline_state* ps = shader_program_get_pipeline_by_state_bits(prog,
        pipe_info->state_bits);
    if (ps) {
        pipeline_state_copy(result, ps);
        return true;
    }

    VkPipeline pipeline = VK_NULL_HANDLE;
    bool success = create_pipeline(&pipeline, pipe_info->state_bits, prog);

    if (!success) {
        log_error("Unable to create pipeline");
        return false;
    }

    if (pipe_info->store_in_program_cache) {
        shader_program_add_pipeline_to_cache(prog, ps);
    }

    return true;
}
