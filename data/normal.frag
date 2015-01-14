#version 140

out vec4 fragColor;

flat in vec3 v_N;
in vec2 v_T;

uniform sampler2D normalsTexture;

void main()
{
	// Task_4_1 - ToDo Begin
	
	// fetch normal from normalsTexture and display as color
	fragColor = vec4(v_N * 0.5 + 0.5, 1.0);
	
	// Task_4_1 - ToDo End
}
