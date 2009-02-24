uniform sampler2D diffuse_map;
uniform sampler2D normal_map;

varying vec3 nLightDir;

vec4 getSpecular(vec3 n)
{
	float nDotHV;
	vec3 halfVector;
		
	halfVector = normalize(gl_ModelViewMatrixInverse * gl_LightSource[0].halfVector).xyz;
	
	nDotHV = max(dot(n, halfVector),0.0);
	
	return gl_FrontMaterial.specular *
	       gl_LightSource[0].specular *
	       pow(nDotHV, gl_FrontMaterial.shininess);
}

void main() 
{
	vec3 n;
	float Lambert;
	vec4 ambient_global, ambient, diffuse, diffuse_tex, specular;
	
//	diffuse_tex = vec4(1, 1, 1, 1);
	diffuse_tex = texture2D(diffuse_map, gl_TexCoord[0].st);
	
	n = texture2D(normal_map, gl_TexCoord[0].st).xyz;
	n = (n - 0.5) * 2.0;
	
    Lambert = max(dot(n, normalize(nLightDir)), 0.0);
    
    ambient_global = gl_LightModel.ambient * gl_FrontMaterial.ambient * diffuse_tex;
    
	if(Lambert > 0.0) {
		ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient * diffuse_tex;
		diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse * Lambert * diffuse_tex;
		specular = getSpecular(n);
	} else {
		ambient = vec4(0, 0, 0, 0);
		diffuse = vec4(0, 0, 0, 0);
		specular = vec4(0, 0, 0, 0);
	}

	gl_FragColor = ambient_global + ambient + diffuse + specular;
}

