#ifndef SHD_H
#define SHD_H

#include "./shader_program/shader_types.h"

#include "./shader_program/pipeline_builder.h"
#include "./shader_program/pipeline_state.h"
#include "./shader_program/render_state.h"
#include "./shader_program/shader.h"
#include "./shader_program/shader_program.h"

#include "./loader/shader_loader.h"
#include "./manager/shader_manager.h"
#include "./shader_program_builder/shader_program_builder.h"

typedef struct shd_tools_create_info {
    size_t max_shader_program_byte_size;
    size_t expected_number_of_shaders;
} shd_tools_create_info;

void create_shader_loader(const shd_tools_create_info* sht_info);
shader_loader* retrieve_shader_loader();
void destroy_shader_loader();

void create_shader_manager(const shd_tools_create_info* sht_info);
shader_manager* retrieve_shader_manager();
void destroy_shader_manager();

#endif
