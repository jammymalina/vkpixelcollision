#include "./render_program.h"

#include "../../../string/string.h"
#include "../../functions/functions.h"

void render_program_init_empty(render_program* prog) {
    string_copy(prog->name, SHADER_MAX_NAME_SIZE, "");
    prog->device = VK_NULL_HANDLE;
    prog->pipeline_layout = VK_NULL_HANDLE;
    prog->descriptor_set_layout = VK_NULL_HANDLE;
}

void render_program_destroy(render_program* prog) {
    if (prog->pipeline_layout) {
        vkDestroyPipelineLayout(prog->device, prog->pipeline_layout, NULL);
        prog->pipeline_layout = VK_NULL_HANDLE;
    }
    if (prog->descriptor_set_layout) {
        vkDestroyDescriptorSetLayout(prog->device, prog->descriptor_set_layout,
            NULL);
        prog->descriptor_set_layout = VK_NULL_HANDLE;
    }
}
