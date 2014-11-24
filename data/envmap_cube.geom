#version 150

// Task_2_3 - ToDo Begin

//uniform mat4 ...[6];
//...

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 g_eye;

void main()
{
	// ToDo: for each cubemap face

	// use the gl_Layer to set the current cube map face to render to
	
	// retrieve the g_eye vector and pass to fragment stage
	
	// set  gl_Position, the input is available via 
	// gl_in[0].gl_Position to gl_in[2].gl_Position
	
	// finish up each vertex with EmitVertex();
	// and each primitive with EmitPrimitivie();
}

// Task_2_3 - ToDo End
