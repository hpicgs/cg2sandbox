#version 150

uniform sampler2D positions;
uniform sampler2D materials0;
uniform sampler2D materials1;

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
bool rcast(in Ray ray, out vec3 normal, out Material material, out float t);

void main()
{
	cache(positions, materials0, materials1); // cache blob and brdf texel fetches
	
	Ray ray;

	// initial ray
	ray.origin    = eye;
	ray.direction = normalize(v_sky);

	Material m; // dummy material - no used
	
	vec3 n; // normal
	float t;

	vec3 c;

	rcast(ray, n, m, t);
	c = n * 0.5 + 0.5;

	fragColor = vec4(c, 1.0);
}
