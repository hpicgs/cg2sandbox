#version 140

uniform mat4 mvp;

in vec2 a_vertex;

out vec2 v_uv;

void main()
{
	v_uv = a_vertex.xy * 0.5 + 0.5;
	v_uv.y = 1.0 - v_uv.y;
	gl_Position = mvp * vec4(a_vertex, 0.0, 1.0);
}
