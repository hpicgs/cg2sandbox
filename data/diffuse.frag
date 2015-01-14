#version 140

out vec4 fragColor;

flat in vec3 v_N;
in vec2 v_T;

uniform sampler2D diffuseTexture;

void main()
{
	// Task_4_1 - ToDo Begin
	
	// fetch diffuse color from diffuseTexture
	fragColor = vec4(1.0, 1.0, 1.0, 1.0);
	
	// Task_4_1 - ToDo End
}
