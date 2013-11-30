#version 140

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
// uniform mat4 ... ?

uniform vec3 light;

in vec3 a_vertex;
in vec3 a_normal;
in vec2 a_texc;

out vec3 v_normal;
out vec2 v_texc;
out vec3 v_eye;
out vec3 v_light;

void main()
{
	v_texc = a_texc;

	v_normal = normalize((view * model * vec4(a_normal, 0.0)).xyz);
	v_eye = normalize((view * model * vec4(a_vertex, 1.0)).xyz);
	v_light = (view * vec4(light, 0.0)).xyz;

	// Task_3_3 - ToDo Begin
	
	// v_shadow = ...;

	// Task_3_3 - ToDo End
	
	gl_Position = projection * view * model * vec4(a_vertex, 1.0);
}
