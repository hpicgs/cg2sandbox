#version 150

uniform sampler2D source;
uniform vec2 size;

in vec2 v_uv;
out vec4 fragColor;

const float sigma = 4.0;

void main()
{
	float u = v_uv.x;
	float v = v_uv.y;

	// Task_5_4 - ToDo Begin

	// ...

    vec4 sum = texture(source, vec2(u, v));
    
	// ...
    
    fragColor = sum;
	
	// Task_5_4 - ToDo End
}
