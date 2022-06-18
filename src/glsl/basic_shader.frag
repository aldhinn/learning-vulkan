#version 450

// Defined previously in the vertex shader.
layout(location = 0) in vec3 frag_colour;
layout(location = 1) in vec2 texture_coordinates;

// The colour to be assigned to the pixels.
layout(location = 0) out vec4 out_colour;

// The texture sampler.
layout(binding = 1) uniform sampler2D texture_sampler;

// Shader entrypoint.
void main() {
    out_colour = vec4(
        frag_colour * texture(texture_sampler, texture_coordinates).rgb,
        1.0
    );
}