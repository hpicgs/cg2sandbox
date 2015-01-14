#version 140

out vec4 fragColor;

flat in vec3 v_N;
in vec3 v_L;
in vec3 v_E;

void main()
{
	vec3 n = normalize(v_N);
	vec3 l = normalize(v_L);

	fragColor = vec4(vec3(max(0.0, dot(l, n))), 1.0);
}
