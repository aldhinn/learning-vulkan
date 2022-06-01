#version 450

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec3 in_colour;

layout(location = 0) out vec3 frag_colour;

// Shader entrypoint.
void main() {
    // Predefined variable gl_Position maps the position in the screen.
    gl_Position = vec4(in_position, 0.0, 1.0);
    frag_colour = in_colour;
}