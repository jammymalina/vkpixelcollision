#ifndef SHADER_TYPES_H
#define SHADER_TYPES_H

#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../../../string/string.h"
#include "../../../collections/vector.h"
#include "../../../collections/hash_string_map.h"

#define SHADER_MAX_NAME_SIZE HASH_KEY_MAX_SIZE
#define SHADER_TYPES_COUNT 7

typedef enum shader_type {
    SHADER_TYPE_VERTEX    =  1,
    SHADER_TYPE_FRAGMENT  =  2,
    SHADER_TYPE_GEOMETRY  =  4,
    SHADER_TYPE_COMPUTE   =  8,
    SHADER_TYPE_TESS_CTRL = 16,
    SHADER_TYPE_TESS_EVAL = 32,
    SHADER_TYPE_UNDEFINED = 64
} shader_type;

typedef enum shadere_program_type {
    SHADER_PROGRAM_TYPE_UNDEFINED = -1,
    SHADER_PROGRAM_TYPE_GRAPHICS,
    SHADER_PROGRAM_TYPE_COMPUTE,
    SHADER_PROGRAM_TYPES_TOTAL
} shader_program_type;

static const shader_type shader_type_index_map[] = {
    SHADER_TYPE_VERTEX, SHADER_TYPE_FRAGMENT, SHADER_TYPE_GEOMETRY,
    SHADER_TYPE_COMPUTE, SHADER_TYPE_TESS_CTRL, SHADER_TYPE_TESS_EVAL,
    SHADER_TYPE_UNDEFINED
};

static inline size_t shader_type_to_index(shader_type st) {
    switch (st) {
        case SHADER_TYPE_VERTEX:
            return 0;
        case SHADER_TYPE_FRAGMENT:
            return 1;
        case SHADER_TYPE_GEOMETRY:
            return 2;
        case SHADER_TYPE_COMPUTE:
            return 3;
        case SHADER_TYPE_TESS_CTRL:
            return 4;
        case SHADER_TYPE_TESS_EVAL:
            return 5;
        case SHADER_TYPE_UNDEFINED:
            return 6;
        default:
            return 0;
    }
}

#define SHADER_TYPE_GROUP_GRAPHICS (SHADER_TYPE_VERTEX | SHADER_TYPE_FRAGMENT)

typedef enum shader_binding_type {
    SHADER_BINDING_TYPE_UNIFORM,
    SHADER_BINDING_TYPE_SAMPLER,
    SHADER_BINDINGS_COUNT
} shader_binding_type;

typedef struct shader_binding {
    shader_binding_type type;
    uint32_t binding;
} shader_binding;

typedef struct shader shader;
typedef struct shader_program shader_program;
typedef struct shader_manager shader_manager;
typedef struct shader_program_manager shader_program_manager;

typedef uint64_t pipeline_state_bits;
typedef struct pipeline_state pipeline_state;
typedef struct pipeline_create_info pipeline_create_info;
typedef struct shader_program_create_info shader_program_create_info;
typedef struct shader_program shader_program;

typedef struct shader_manager_program_create_info
    shader_manager_program_create_info;
typedef struct shader_builder_program_create_info
    shader_builder_program_create_info;

static inline shader_type shader_extension_to_type(const char* extension) {
    if (string_equal(extension, "vert"))
        return SHADER_TYPE_VERTEX;
    if (string_equal(extension, "frag"))
        return SHADER_TYPE_FRAGMENT;
    if (string_equal(extension, "tesc"))
        return SHADER_TYPE_TESS_CTRL;
    if (string_equal(extension, "tese"))
        return SHADER_TYPE_TESS_EVAL;
    if (string_equal(extension, "geom"))
        return SHADER_TYPE_GEOMETRY;
    if (string_equal(extension, "comp"))
        return SHADER_TYPE_COMPUTE;

    return SHADER_TYPE_UNDEFINED;
}

static inline const char* shader_type_to_extension(shader_type type) {
    switch (type) {
        case SHADER_TYPE_VERTEX:
            return "vert";
        case SHADER_TYPE_FRAGMENT:
            return "frag";
        case SHADER_TYPE_TESS_CTRL:
            return "tesc";
        case SHADER_TYPE_TESS_EVAL:
            return "tese";
        case SHADER_TYPE_GEOMETRY:
            return "geom";
        case SHADER_TYPE_COMPUTE:
            return "comp";
        default:
            return "undefined";
    }
}

static inline VkShaderStageFlagBits shader_type_to_stage(shader_type type) {
    switch (type) {
        case SHADER_TYPE_VERTEX:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case SHADER_TYPE_FRAGMENT:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case SHADER_TYPE_TESS_CTRL:
            return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case SHADER_TYPE_TESS_EVAL:
            return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        case SHADER_TYPE_GEOMETRY:
            return VK_SHADER_STAGE_GEOMETRY_BIT;
        case SHADER_TYPE_COMPUTE:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        default:
            return VK_SHADER_STAGE_ALL;
    }
}

static inline VkDescriptorType shader_binding_type_to_descriptor_type(
    shader_binding_type type)
{
    switch (type) {
        case SHADER_BINDING_TYPE_UNIFORM:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case SHADER_BINDING_TYPE_SAMPLER:
            return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        default:
            return VK_DESCRIPTOR_TYPE_MAX_ENUM;
    }
}

#endif
