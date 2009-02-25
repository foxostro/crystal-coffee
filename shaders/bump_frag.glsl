uniform sampler2D diffuse_map;
uniform sampler2D normal_map;

varying vec3 LightDir;
varying vec3 EyeDir;

void main() 
{	
	vec4 diffuse_tex = texture2D(diffuse_map, gl_TexCoord[0].st);
	
	vec3 n = texture2D(normal_map, gl_TexCoord[0].st).xyz;
	n = (n - 0.5) * 2.0;
	
    float Lambertian = max(dot(n, LightDir), 0.0);
    
	vec4 litColor = Lambertian * diffuse_tex; // gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse
	
	/*
	vec3 reflectDir = reflect(LightDir, n);
	float spec = max(dot(EyeDir, reflectDir), 0.0);
	spec = pow(spec, 10.0);
	spec *= 0.5;
	litColor = min(litColor + spec, vec4(1.0));
	*/

	gl_FragColor = litColor;
}

