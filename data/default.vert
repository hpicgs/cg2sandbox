#version 140

uniform mat4 transform;

in vec3 a_vertex;
in vec3 a_normal;

flat out vec3 v_normal;

void main()
{
	v_normal = a_normal;
	gl_Position = transform * vec4(a_vertex, 1.0);
}
