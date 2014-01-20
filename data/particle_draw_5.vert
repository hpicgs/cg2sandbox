#version 150

uniform mat4 view;
uniform mat4 projectionInverted;
uniform mat4 viewProjectionInverted;

in vec2 a_vertex;

out vec2 v_uv;
out vec3 v_ray;
out vec3 v_sky;
 
void main()
{
	v_uv = a_vertex * 0.5 + 0.5;
	v_ray = (viewProjectionInverted * vec4(a_vertex, 1.0, 1.0)).xyz;
	v_sky = (projectionInverted * vec4(a_vertex, 0.0, 1.0) * view).xyz;

	gl_Position = vec4(a_vertex, 1.0, 1.0);
}
 