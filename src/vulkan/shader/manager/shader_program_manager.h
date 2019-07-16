#ifndef SHADER_PROGRAM_MANAGER_H
#define SHADER_PROGRAM_MANAGER_H

#include <stdbool.h>
#include <stddef.h>

#include "../../../collections/hash_string_map.h"
#include "../../gpu/gpu.h"
#include "../shader_program/shader_types.h"
#include "../shader_program/shader_program.h"

#define DEFAULT_EXPECTED_NUMBER_OF_SHADER_PROGRAMS 10

typedef struct shader_program_hash_map HASH_STRING_MAP(shader_program)
    shader_program_hash_map;

typedef struct shader_program_manager_create_info {
    size_t expected_number_of_shader_programs;
} shader_program_manager_create_info;

typedef struct shader_manager_program_create_info {
    const char* name;
    shader_program_type type;
    vertex_layout_type vertex_layout;

    const gpu_info* gpu;

    const char** shaders;
    size_t shaders_size;

    VkRenderPass default_render_pass;
    pipeline_create_info* preconfigured_pipelines;
    size_t preconfigured_pipelines_size;
} shader_manager_program_create_info;

typedef struct shader_program_preload_info {
    const gpu_info* default_gpu;
    VkRenderPass default_render_pass;
    const shader_manager_program_create_info* shader_programs_config;
    size_t shader_programs_config_size;
} shader_program_preload_info;

typedef struct shader_program_manager {
    shader_program_hash_map programs;
} shader_program_manager;

void shader_program_manager_init_empty(shader_program_manager* spm);
bool shader_program_manager_init(shader_program_manager* spm, const
    shader_program_manager_create_info* spm_info);

bool shader_program_manager_add(shader_program_manager* spm, const char*
    shader_name, const shader_program* program);
bool shader_program_manager_get(shader_program_manager* spm, const char*
    shader_name, shader_program* program);
shader_program* shader_program_manager_get_reference(const
    shader_program_manager* spm, const char* shader_name);
bool shader_program_manager_has(shader_program_manager* spm, const char*
    shader_name);
bool shader_program_manager_delete(shader_program_manager* spm, const char*
    shader_name);

bool shader_program_manager_preload(shader_program_manager* spm, const
    shader_program_preload_info* preload_info);

void shader_program_manager_destroy(shader_program_manager* spm);

#endif
