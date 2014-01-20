#version 150

in vec2 a_vertex;

void main()
{
	gl_Position = vec4(a_vertex, 1.0, 1.0);
}
  