#version 140

uniform sampler2D height;

uniform mat4 view;
uniform mat4 mvp;

uniform float yOffset;
uniform float yScale;

uniform Transforms
{
	mat4 T[1024];
};

in vec2 a_vertex;

out vec2 v_texc;
out vec3 v_eye;
out vec3 v_light;

void main()
{
	vec4 v = T[gl_InstanceID] * vec4(a_vertex.x, 0.0, a_vertex.y, 1.0);
	v_texc = 0.125 * v.xz + 0.5;

	v.y = texture(height, v_texc).r * yScale * 1 + yOffset * 1;

	v_eye = v.xyz;
	gl_Position = mvp * v;
}
