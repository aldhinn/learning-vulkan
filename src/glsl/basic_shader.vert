#version 450

layout(binding = 0) uniform Uniform {
    mat4 model;
    mat4 view;
    mat4 projection;
} bound_uniform;

// The first 3 floats layed out in the vertex input are the 3D positions.
layout(location = 0) in vec3 in_3D_position;
// Th next 3 floats layed out in the vertex
// input are the normalized RGB colour value.
layout(location = 1) in vec3 in_colour;
// The final 2 floats layed out in the vertex
// input are the texture coordinates.
layout(location = 2) in vec2 in_texture_coordinates;

// To be passed to the next shader stage,
// which in our case is the fragment shader.
layout(location = 0) out vec3 out_frag_colour;
layout(location = 1) out vec2 out_texture_coordinates;

// Shader entrypoint.
void main() {
    // gl_Position is a built in shader variable specifying the vertex position.
    gl_Position = bound_uniform.projection * bound_uniform.view *
        bound_uniform.model * vec4(in_3D_position, 1.0);
    out_frag_colour = in_colour;
    out_texture_coordinates = in_texture_coordinates;
}