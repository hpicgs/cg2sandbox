#version 150

uniform sampler2D ground;

out vec4 fragColor;

// Task_1_3 - ToDo Begin

// Note: you can specify consts instead of uniforms or local variables...
//const float t = 1.0; 

void main()
{
	// Implement height based texturing using a texture atlas
	// with known structure ;)
	
	// Tip: start with getting the height from the vertex stage 
	// and think of a function that yields you the two texture "indices".
	// These "indices" can be used as offset when accessing the texture...
	// Tip: again, checkout step, clamp, mod, and smoothstep!

	// ...
	
	// Note: its ok if you habe minimal seams..., you need not to apply
	// mipmaps not to use safe margins to avoid those seems. 
	
	// ...
	
	// Note: feel free to scale the textures as it pleases you.
	
	// ...
	
	//float i = ... ;
	//fragColor = mix(texture2D(ground, uv0), texture2D(ground, uv1), i);
	
	fragColor = vec4(vec3(0.0), 1.0);
	
	// Task_1_3 - ToDo End
}


