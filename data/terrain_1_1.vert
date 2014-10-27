#version 150

// Task_1_1 - ToDo Begin

uniform mat4 transform;

// in ### a_vertex;

void main()
{
	gl_Position = transform * vec4(0.0, 0.0, 0.0, 1.0);

	// Task_1_1 - ToDo End
}
