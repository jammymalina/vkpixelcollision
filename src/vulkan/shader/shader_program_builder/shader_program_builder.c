#include "./shader_program_builder.h"

#include "../../../logger/logger.h"
#include "../../functions/functions.h"
#include "../../tools/tools.h"
#include "../shd.h"

static bool shader_builder_prepare_program(shader_program* prog, const
    shader_program_create_info* prog_info)
{
    return shader_program_init(prog, prog_info);
}

bool shader_builder_preload_programs(shader_program* prog, const
    shader_manager_program_create_info* prog_info)
{
    shader_program_create_info prep_info = {
        .gpu = prog_info->gpu,
        .shaders = prog_info->shaders,
        .shaders_size = prog_info->shaders_size,
        .vertex_layout = prog_info->vertex_layout
    };
    bool success = shader_builder_prepare_program(prog, &prep_info);
    ASSERT_LOG_ERROR(success, "Unable to build shader program");

    for (size_t i = 0; i < prog_info->preconfigured_pipelines_size; ++i) {
        pipeline_state tmp;
        pipeline_create_info pipe_info = prog_info->preconfigured_pipelines[i];
        pipe_info.render_pass = pipe_info.render_pass ?
            pipe_info.render_pass : prog_info->default_render_pass;
        success = pipeline_builder_build_pipeline(&tmp, prog,
            &pipe_info);
        ASSERT_LOG_ERROR(success, "Unable to create pipeline");
    }

    return true;
}

bool shader_builder_build_program(shader_program* prog, pipeline_state*
    result_pipeline, const shader_builder_program_create_info* prog_info)
{
    return true;
}
