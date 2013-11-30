#version 140

uniform sampler2D source;

out vec4 fragColor;

in vec2 v_uv;

void main()
{
	float s = texture(source, v_uv).r;
	fragColor = vec4(vec3(pow(s, 32.0)), 1.0);
}
