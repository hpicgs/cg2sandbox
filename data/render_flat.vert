#version 140

uniform mat4 viewProjection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 normalMatrix;
uniform vec3 camera;
uniform vec3 light;

in vec3 a_vertex;
in vec3 a_normal;
in vec2 a_texCoord;

flat out vec3 v_N;
out vec3 v_L;
out vec3 v_E;
out vec2 v_T;

void main()
{
    gl_Position = viewProjection * model * vec4(a_vertex, 1.0);
	
    vec4 eyePos   = model * vec4(a_vertex, 1.0);
    vec4 lightPos = vec4(light, 1.0);
    
    v_N = a_normal;
    v_L = lightPos.xyz - eyePos.xyz;
    v_E = -eyePos.xyz;
    v_T = a_texCoord;
}
