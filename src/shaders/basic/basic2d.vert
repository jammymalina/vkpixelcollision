#version 460
#extension GL_ARB_separate_shader_objects: enable

layout (location = 0) in vec2 position;
layout (location = 1) in vec4 color;


out gl_PerVertex {
    vec4 gl_Position;
};

layout (location = 0) out vec4 frag_color;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    frag_color = color;
}
