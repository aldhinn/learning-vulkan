#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 frag_colour;

// Shader entrypoint.
void main() {
    // Predefined variable gl_Position maps the position in the screen.
    gl_Position = vec4(inPosition, 0.0, 1.0);
    frag_colour = inColor;
}