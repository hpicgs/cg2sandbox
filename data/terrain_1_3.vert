#version 150

// Task_1_3 - ToDo Begin

uniform mat4 transform;
uniform sampler2D height;

// in ### a_vertex;

// out ... ;

void main()
{
	// Note: should be similar to 1_2 vertex shader.
	// In addition, you need to pass the texture coords 
	// to the fragment shader...

	// ...

	gl_Position = transform * vec4(0.0, 0.0, 0.0, 1.0);
	
	// Task_1_3 - ToDo End
}
