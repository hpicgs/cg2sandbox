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
bool rcast(in Ray ray, out vec3 normal, out Material material, out float t);
vec3 CookTorrance(in vec3 V, in vec3 N, in vec3 L, in Material m, in vec3 R, in vec3 ambient);

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

	if(rcast(ray, n, m, t))
	{
		vec3 v = -ray.direction;; // view

		vec3 l = light;
		vec3 R = texture(envmap, reflect(ray.direction, n)).xyz;
		c = CookTorrance(v, n, l, m, R, ambient);
	}
	else
		c = texture(envmap, ray.direction).xyz;

	fragColor = vec4(c, 1.0);
}
