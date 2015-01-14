#version 140

in vec2 a_vertex;

out vec2 coord;

void main()
{
    gl_Position = vec4(a_vertex, 0.0, 1.0);
    coord = a_vertex * 0.5 + 0.5;
}
