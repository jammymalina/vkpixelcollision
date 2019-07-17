#include "./vertex_layout.h"

#include "../../../memory/memory.h"

static void vertex_layout_init_empty(vertex_layout* vl) {
     VkPipelineVertexInputStateCreateInfo empty_vertex_input_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = NULL,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = NULL
    };
    vl->input_state = empty_vertex_input_info;

    mem_set(vl->binding_desc, 0, sizeof(VkVertexInputBindingDescription) *
        MAX_VERTEX_BINDING_DESCRIPTORS);
    vl->binding_desc_size = 0;

    mem_set(vl->attribute_desc, 0, sizeof(VkVertexInputAttributeDescription) *
        MAX_VERTEX_ATTRIBUTE_BINDING_DESCRIPTORS);
    vl->attribute_desc_size = 0;
}

static void vertex_layout_init_pos_2(vertex_layout* vl) {
    vl->binding_desc_size = 1;
    vl->binding_desc[0].binding = 0;
    vl->binding_desc[0].stride = 2 * sizeof(float);
    vl->binding_desc[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    vl->attribute_desc_size = 1;
    // position
    vl->attribute_desc[0].location = 0;
    vl->attribute_desc[0].binding = vl->binding_desc[0].binding,
    vl->attribute_desc[0].format = VK_FORMAT_R32G32_SFLOAT,
    vl->attribute_desc[0].offset = 0;
}

static void vertex_layout_init_pos_2_col_4(vertex_layout* vl) {
    vl->binding_desc_size = 1;
    vl->binding_desc[0].binding = 0;
    vl->binding_desc[0].stride = 2 * sizeof(float) + 4 * sizeof(float);
    vl->binding_desc[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    vl->attribute_desc_size = 2;
    // position
    vl->attribute_desc[0].location = 0;
    vl->attribute_desc[0].binding = vl->binding_desc[0].binding,
    vl->attribute_desc[0].format = VK_FORMAT_R32G32_SFLOAT,
    vl->attribute_desc[0].offset = 0;
    // color
    vl->attribute_desc[1].location = 1;
    vl->attribute_desc[1].binding = vl->binding_desc[0].binding,
    vl->attribute_desc[1].format = VK_FORMAT_R32G32B32A32_SFLOAT,
    vl->attribute_desc[1].offset = 2 * sizeof(float);
}

static void vertex_layout_init_pos_uv_2(vertex_layout* vl) {
    vl->binding_desc_size = 1;
    vl->binding_desc[0].binding = 0;
    vl->binding_desc[0].stride = 2 * sizeof(float) + 2 * sizeof(float);
    vl->binding_desc[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    vl->attribute_desc_size = 2;
    // position
    vl->attribute_desc[0].location = 0;
    vl->attribute_desc[0].binding = vl->binding_desc[0].binding,
    vl->attribute_desc[0].format = VK_FORMAT_R32G32_SFLOAT,
    vl->attribute_desc[0].offset = 0;
    // color
    vl->attribute_desc[1].location = 1;
    vl->attribute_desc[1].binding = vl->binding_desc[0].binding,
    vl->attribute_desc[1].format = VK_FORMAT_R32G32_SFLOAT,
    vl->attribute_desc[1].offset = 2 * sizeof(float);
}

void retrieve_vertex_layouts(vertex_layout
    vertex_layouts[VERTEX_LAYOUTS_TOTAL])
{
    for (size_t i = 0; i < VERTEX_LAYOUTS_TOTAL; ++i) {
        vertex_layout_init_empty(&vertex_layouts[i]);
    }

    vertex_layout_init_pos_2(&vertex_layouts[VERTEX_LAYOUT_POS_2]);
    vertex_layout_init_pos_2_col_4(&vertex_layouts[VERTEX_LAYOUT_POS_2_COL_4]);
    vertex_layout_init_pos_uv_2(&vertex_layouts[VERTEX_LAYOUT_POS_UV_2]);
}
