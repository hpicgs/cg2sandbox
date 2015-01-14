#version 140

out vec4 color;

// Task_4_2 - Task 4_3 - ToDo Begin

// add sampler2D uniforms for "diffuseTexture", "normalsTexture", "specularTexture" and "illuminationTexture"
// uniform sampler2D ...;

// Task_4_2 - Task 4_3 - ToDo End

uniform vec3 camera;
uniform float timef;

in vec3 v_N;
in vec3 v_L;
in vec3 v_E;
in vec2 v_T;

// Task_4_2 - Task 4_3 - ToDo Begin

// Adjust factors
const float ambientFactor = 0.0;
const float diffuseFactor = 1.0;
const float specularFactor = 0.0;
const float emissionFactor = 0.0;

// Task_4_2 - Task 4_3 - ToDo End

void main()
{ 
	// Task_4_2 - ToDo Begin
	
	// Implement phong shading using the information from the material textures
	// must have components are diffuse, specular (including shininess) and emissive
	// each component has to be configurable with the factors defined above (e.g., diffuseFactor)
	
	// Task_4_3 - ToDo Begin
	
	// use normal information from texture and project the light vector into tangent space
	// therefore a transformation matrix called "tbn" is needed
	// The needed vectors can be computed using dFdx and dFdy
	
	// Task_4_3 - ToDo End
	
	float labertTerm = max(0.0, dot(normalize(v_L), normalize(v_N)));
	
	color = vec4(vec3(labertTerm * diffuseFactor), 1.0);
	
	// Task_4_2 - ToDo End
}
