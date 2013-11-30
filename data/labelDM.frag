#version 140

uniform sampler2D label;

out vec4 fragColor;

in vec2 v_uv;

float aastep(float threshold, float value) 
{
	// Task_3_2 - ToDo Begin

	// ToDo: implement this function by using 
	// the GLSL functions dFdx and dFdy ;) and smoothstep
	
	// The result should provide a fragment width dependent
	// smoothing, that is independend of the actual viewports
	// dimensions...

	return 0.0;
	
	// Task_3_2 - ToDo End
}

void main()
{
	// Task_3_2 - ToDo Begin

	float a = texture(label, v_uv).r;
	fragColor = vec4(vec3(a), 1.0);
	
	// Task_3_2 - ToDo End
}
