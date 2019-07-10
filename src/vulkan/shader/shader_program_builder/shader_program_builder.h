#ifndef SHADER_PROGRAM_BUILDER_H
#define SHADER_PROGRAM_BUILDER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../../gpu/gpu.h"
#include "../shader_program/shader_types.h"
#include "../shader_program/shader_program.h"

bool shader_builder_build_shader_program(shader_program* prog, const
    shader_program_create_info* prog_info);

#endif
