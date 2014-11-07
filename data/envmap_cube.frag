#version 140

uniform sampler2D envmap;
uniform samplerCube cubemap;

uniform int mapping;

out vec4 fragColor;

in vec3 g_eye;

// Feel free to copy/paste from Task_2_2 or modify...
// Task_2_3 - ToDo Begin


void main()
{
	vec3 eye = normalize(g_eye);
	//vec4 color = env(eye);
	
	//fragColor = color;
}

// Task_2_3 - ToDo End
