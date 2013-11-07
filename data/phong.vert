#version 140

uniform mat4 transform;

in vec3 a_vertex;
in vec3 a_normal;

varying vec3 N;
varying vec3 L;
varying vec3 E;
                                            
void main(void)
{
   gl_Position 	= transform * vec4(a_vertex, 1.0);
   
   vec4 eye 	= gl_ModelViewProjectionMatrix * gl_Vertex;
   vec4 light 	= gl_LightSource[0].position;
                                                                                                                   
   N = normalize(gl_NormalMatrix * a_normal);
   L = light.xyz - eye.xyz;
   E = -eye.xyz;
}
