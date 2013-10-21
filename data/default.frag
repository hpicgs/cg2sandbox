#version 140

out vec4 fragColor;

flat in vec3 v_normal;

void main()
{
	vec3 normal = normalize(v_normal);
	fragColor = vec4(normal * 0.5 + 0.5, 1.0);
}
