#version 140

uniform samplerCube source;

uniform float timef;

out vec4 fragColor;

in vec3 v_normal;
in vec3 v_eye;

// Task_2_3 - ToDo Begin

void main()
{
	vec3 n = normalize(v_normal);
	vec3 e = normalize(v_eye);
	
	//vec3 r = ...
	//vec3 q = ...
	
	// float frsl = ...;

	// vec4 refl = ...(r);
	// vec4 refr = ...(q);
	
	// fragColor = ...;
}

// Task_2_3 - ToDo End