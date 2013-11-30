#version 140

uniform vec4 diffuse;
uniform vec4 ambient;
uniform vec4 specular;
uniform vec4 emissive;

uniform float shininess; // does not work correct for me (DL) in release mode

uniform sampler2D difftex;
uniform sampler2D shadowmap;
uniform int texCount;
uniform bool useshadow;

out vec4 fragColor;

in vec3 v_normal;
in vec2 v_texc;
in vec3 v_eye;
in vec3 v_light;

void main()
{
	vec3 n = normalize(v_normal);
	vec3 e = normalize(v_eye);
	vec3 l = normalize(v_light);
	vec3 r = reflect(e, n);

	float ldotn = max(dot(l, n), 0.0); 
	float rdotn = max(dot(r, l), 0.0);

	vec4 s = specular * clamp(pow(rdotn, shininess), 0.0, 1.0); 

	vec4 d = diffuse;
	if(texCount > 0)
		d = texture(difftex, v_texc);

	float shadow = 1.0;
	if(useshadow)
	{
		// Task_3_3 - ToDo Begin
	
		// shadow = ...?

		// Task_3_3 - ToDo End
	}

	fragColor = vec4(mix(ambient.xyz * d.xyz, d.xyz * shadow, ldotn) + s.xyz + emissive.xyz, 1.0);
}
