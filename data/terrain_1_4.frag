#version 140

uniform sampler2D ground;

out vec4 fragColor;

// Task_1_4 - ToDo Begin

// Note: start with a copy of your 1_3 shader and add the lambert term 
// based on a static light source (use const variable)...

void main()
{
	//fragColor = lambert * /*as before ... */;

	fragColor = vec4(vec3(0.0), 1.0);
	
	// Task_1_4 - ToDo End
}
