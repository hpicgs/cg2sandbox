#version 140

uniform mat4 viewProjection;
uniform mat4 model;

in vec3 a_vertex;

void main()
{
	gl_Position = viewProjection * model * vec4(a_vertex, 1.0);
}
