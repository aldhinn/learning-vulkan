#version 450

layout(location = 0) out vec3 frag_colour;

// Vertex positions in terms of normalized device coordinates.
vec2 positions[3] = vec2[](
    vec2(-0.5, -0.5),
    vec2(0.5, -0.5),
    vec2(0.0, 0.5)
);

// Colour of the pixel on the vertex coordinate.
vec3 colours[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

// Shader entrypoint.
void main() {
    // Predefined variable gl_Position maps the position in the screen.
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    frag_colour = colours[gl_VertexIndex];
}