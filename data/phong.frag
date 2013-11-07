varying vec3 N;
varying vec3 L;
varying vec3 E;
                                                                                                                   
void main()
{
	vec3 norm 	= normalize(N);
	vec3 lightv = normalize(L);
	vec3 viewv 	= normalize(E);
	vec3 halfv 	= normalize(lightv + viewv);
	
	float f;
	if (dot(lightv, norm)>= 0.0)
		f =1.0;
	else
		f = 0.0;

	float Kd = max(0.0, dot(lightv, norm));
	float Ks = pow(max(0.0, dot(norm, halfv)), gl_FrontMaterial.shininess);
	
	vec4 diffuse 	= Kd * gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
	vec4 ambient 	= gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
	vec4 specular 	= f * Ks * gl_FrontMaterial.specular * gl_LightSource[0].specular;
	
	gl_FragColor = ambient + diffuse + specular;
}