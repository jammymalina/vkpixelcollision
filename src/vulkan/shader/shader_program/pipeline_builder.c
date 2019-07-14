#include "./pipeline_builder.h"

#include "../../../logger/logger.h"
#include "../../functions/functions.h"
#include"../../tools/tools.h"
#include "../../vertex/core/vertex_layout.h"

static bool create_pipeline(VkPipeline *pipeline, pipeline_state_bits
    state_bits, VkRenderPass render_pass, shader_program* prog)
{
    vertex_layout vertex_layouts[VERTEX_LAYOUTS_TOTAL];
    retrieve_vertex_layouts(vertex_layouts);
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
    pipeline_state_get_dynamic_states(&ps, dynamic_states,
        &dynamic_states_size);

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
        .renderPass = render_pass,
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
    const pipeline_state* ps = shader_program_get_pipeline(prog,
        pipe_info->state_bits, pipe_info->render_pass);
    if (ps) {
        pipeline_state_copy(result, ps);
        return true;
    }

    pipeline_state_init_empty(result);

    VkPipeline pipeline = VK_NULL_HANDLE;
    bool success = create_pipeline(&pipeline, pipe_info->state_bits,
        pipe_info->render_pass, prog);

    if (!success) {
        log_error("Unable to create pipeline");
        return false;
    }

    result->gpu = prog->gpu;
    result->handle = pipeline;
    result->render_pass = pipe_info->render_pass;
    result->state_bits = pipe_info->state_bits;

    if (pipe_info->store_in_program_cache) {
        shader_program_add_pipeline_to_cache(prog, result);
    }

    return true;
}
