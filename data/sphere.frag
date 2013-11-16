#version 140

uniform sampler2D envmap;
uniform samplerCube cubemap;

uniform int mapping;
uniform float timef;

out vec4 fragColor;

in vec3 v_normal;
in vec3 v_eye;


// Task_2_2 - ToDo Begin

// Copy your env function from 2_1

void main()
{
	vec3 n = normalize(v_normal);
	vec3 e = normalize(v_eye);
	
	//vec3 r = ...
	//vec3 q = ...
	
	// float frsl = ...;

	// vec4 refl = env(r);
	// vec4 refr = env(q);
	
	// fragColor = ...;
}

// Task_2_2 - ToDo End
