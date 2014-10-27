#version 150

// Task_1_2 - ToDo Begin

uniform mat4 transform;
uniform sampler2D height;

// in ### a_vertex;

// out ... ;

void main()
{
	// Here you need to gather texture coordinates.
	// Tip: note that your terrain might/should be in range [0,1] which 
	// accidentally could be interpreted as texture coordinates ...
	
	// use texture function to access a sampler2D object
	
	// ...

	gl_Position = transform * vec4(0.0, 0.0, 0.0, 1.0);
	
	// Task_1_2 - ToDo End
}
