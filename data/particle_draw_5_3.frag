#version 150

uniform sampler2D positions;
uniform sampler2D materials0;
uniform sampler2D materials1;
uniform samplerCube envmap;

uniform vec3 eye;

in vec2 v_uv;
in vec3 v_ray;
in vec3 v_sky;

out vec4 fragColor;

struct Sphere
{
	vec3 position;
	float radius;
};

struct Ray
{
	vec3 origin;
	vec3 direction;
};

struct Material
{
	vec4 sr; // vec3 specular, float reflectance
	vec4 dr; // vec3 diffuse, float roughness
};

const vec3 light = normalize(vec3(1.0, 0.0, -1.0));
const vec3 ambient = vec3(0.3725, 0.3686, 0.4314);


void cache(sampler2D positions, sampler2D materials0, sampler2D materials1);
bool trace(in Ray ray, out vec3 normal, out Material material, out float t);
vec3 CookTorrance(in vec3 V, in vec3 N, in vec3 L, in Material m, in vec3 R, in vec3 ambient);


void main()
{
	cache(positions, materials0, materials1); // cache blob and brdf texel fetches
	
	Ray ray[3];
	vec3 h[2];

	// initial ray
	ray[0].origin    = eye;
	ray[0].direction = normalize(v_sky);
	
	vec3 	 l = normalize(light); // -ray[0].direction; 
	vec3     n[2];
	Material m[2];

	float t;
	vec3 c;
	
	float lum = 0.0;
	
	// Task_5_3 - ToDo Begin
	// The following code is just a suggestion, feel free to adjust or modify it!

	if(trace(ray[0], n[0], m[0], t))
	{
		vec3 v[2];

		// ...

		// e.g. use ray[1]... ray[2]...
		
		vec3 R;
	
		if(trace(ray[1], n[1], m[1], t))
		{
			R = vec3(0.0); // CookTorrance(...);
		}
		else
			R = texture(envmap, ray[1].direction).xyz;

		c = vec3(0.0); // CookTorrance(...);
	}
	else
	{
		c = texture(envmap, ray[0].direction).xyz;
		// lum = ;
	}	
	
	// Task_5_3 - ToDo End
	
	fragColor = vec4(c, lum);
}
