#version 140

uniform sampler2D envmap;
uniform samplerCube cubemap;

uniform int mapping;

out vec4 fragColor;

in vec3 v_eye;

// Task_2_1 - ToDo Begin
// Implement the four requested projection mappings.

// const float c_...  = ...;

vec4 env(in vec3 eye)
{
	vec4 color;
	
	if(0 == mapping) 		// cube
	{
		// use texture function with the cubemap sampler
		color = vec4(1.0, 0.0, 0.0, 1.0); // ToDo
	}
	else if(1 == mapping) 	// polar
	{	
		// use texture function with the envmap sampler
		color = vec4(0.0, 1.0, 0.0, 1.0); // ToDo
	}	
	else if(2 == mapping) 	// paraboloid
	{
		// use texture function with the envmap sampler
		color = vec4(0.0, 0.0, 1.0, 1.0); // ToDo
	}
	else if(3 == mapping) 	// sphere
	{
		// use texture function with the envmap sampler
		color = vec4(1.0, 1.0, 0.0, 1.0); // ToDo
	}
	return color;
}

// Task_2_1 - ToDo End


void main()
{
	vec3 eye = normalize(v_eye);
	vec4 color = env(eye);
	
	fragColor = color;
}
