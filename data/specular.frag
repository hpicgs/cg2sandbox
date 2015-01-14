#version 140

out vec4 fragColor;

flat in vec3 v_N;
in vec2 v_T;

uniform sampler2D specularTexture;

void main()
{
	// Task_4_1 - ToDo Begin
	
	// fetch specular color from specularTexture
	fragColor = vec4(0.0, 0.0, 0.0, 1.0);
	
	// Task_4_1 - ToDo End
}
