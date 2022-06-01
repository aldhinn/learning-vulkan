#version 450

// The first 2 floats layed out in the vertex input are the 2D positions.
layout(location = 0) in vec2 in_2D_positions;
// The final 3 floats layed out in the vertex
// input are the normalized RGB colour value.
layout(location = 1) in vec3 in_colour;

// To be passed to the next shader stage,
// which in our case is the fragment shader.
layout(location = 0) out vec3 out_frag_colour;

// Shader entrypoint.
void main() {
    // gl_Position is a built in shader variable specifying the vertex position.
    gl_Position = vec4(in_2D_positions, 0.0, 1.0);
    out_frag_colour = in_colour;
}