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

#define SHADER_TYPE_GROUP_GRAPHICS (SHADER_TYPE_VERTEX | SHADER_TYPE_FRAGMENT)

typedef enum shader_binding {
    SHADER_BINDING_TYPE_UNIFORM,
    SHADER_BINDING_TYPE_SAMPLER,
    SHADER_BINDINGS_COUNT
} shader_binding;

typedef struct shader shader;

typedef uint64_t pipeline_state_bits;
typedef struct shader_program shader_program;
typedef struct pipeline_cache pipeline_cache;

typedef struct shader_vector VECTOR(shader*) shader_vector;

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

#endif
