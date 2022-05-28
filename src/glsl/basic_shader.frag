#version 450

// Defined previously in the vertex shader.
layout(location = 0) in vec3 frag_colour;

// The colour to be assigned to the pixels.
layout(location = 0) out vec4 out_colour;

// Shader entrypoint.
void main() {
    out_colour = vec4(frag_colour, 1.0);
}