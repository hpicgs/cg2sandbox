#version 150

uniform sampler2D positions;
uniform sampler2D velocities;
uniform sampler3D forces;
uniform float elapsed; // time delta

out vec4 fboPositions;
out vec4 fboVelocities;

const float gravity  = 1.0;
const float friction = 0.2;

void moveParticlesInForceField(
	in vec4 in_position
, 	in vec4 in_velocity
, 	in float elapsed
, 	in sampler3D forces
, 	out vec4 out_position
, 	out vec4 out_velocity)
{
	vec3 p = in_position.xyz;
	vec3 v = in_velocity.xyz;

	vec3 force = texture(forces, p * 0.5 + 0.5).xyz;

	float t = elapsed;
	vec3 g = sign(-p) * (p * p); // gravity to center

	vec3 f  = (g * gravity + force) - (v * friction);

	out_position = vec4(p + (v * t) + (0.5 * f * t * t), in_position.w);
	out_velocity = vec4(v + (f * t), 1.0);
}

void main()
{
	vec4 pos = texelFetch(positions,  ivec2(int(gl_FragCoord.x), int(gl_FragCoord.y)), 0);	
	vec4 vel = texelFetch(velocities, ivec2(int(gl_FragCoord.x), int(gl_FragCoord.y)), 0);

	vec4 posOut;
    vec4 velOut;

	moveParticlesInForceField(pos, vel, elapsed, forces, posOut, velOut);

	fboPositions  = posOut;
	fboVelocities = velOut;
}
