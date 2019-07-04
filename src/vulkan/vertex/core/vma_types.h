#ifndef VERTEX_TYPES_H
#define VERTEX_TYPES_H

#include <stddef.h>
#include <vulkan/vulkan.h>

#define MAX_VERTEX_BINDING_DESCRIPTORS 8
#define MAX_VERTEX_ATTRIBUTE_BINDING_DESCRIPTORS 8

typedef enum vertex_layout_type {
	VERTEX_LAYOUT_UNKNOWN = -1,
    VERTEX_LAYOUT_NO_VERTICES,
    VERTEX_LAYOUT_POS_2,
	VERTEX_LAYOUT_POS_NOR_3,
	VERTEX_LAYOUT_POS_NOR,
    VERTEX_LAYOUT_POS_NOR_UV_3,
    VERTEX_LAYOUT_POS_NOR_UV,
	VERTEX_LAYOUTS_TOTAL
} vertex_layout_type;

typedef struct vertex_layout {
	VkPipelineVertexInputStateCreateInfo input_state;

	VkVertexInputBindingDescription
        binding_desc[MAX_VERTEX_BINDING_DESCRIPTORS];
    size_t binding_desc_size;

	VkVertexInputAttributeDescription
        attribute_desc[MAX_VERTEX_ATTRIBUTE_BINDING_DESCRIPTORS];
    size_t attribute_desc_size;
} vertex_layout;

void retrieve_vertex_layouts(vertex_layout
    vertex_layouts[VERTEX_LAYOUTS_TOTAL]);

#endif
