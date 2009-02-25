uniform sampler2D diffuse_map;
uniform sampler2D normal_map;

varying vec3 vertex_to_light;
varying vec3 eye_to_vertex;

const float spec_multiplier = 3.0;

void main()
{
	// Get the color of the diffuse texture at this fragment
	vec4 diffuse_map_color = texture2D(diffuse_map, gl_TexCoord[0].st);
	
	/* Get the perturbed normal from the normal map.
	 * This normal can be used in the Lambert term almost directly. We need to
	 * unpack it from the texture, though.
	 */
	vec3 N = vec3(0);
	N = texture2D(normal_map, gl_TexCoord[0].st).xyz;
	N = ((N - 0.5) * 2.0);

	/* The vertex_to_light vector encodes the distance to the light, so we need
	 * to normalize before using it in the Lambert term.
	 */
	vec3 L = normalize(vertex_to_light);

	/* Calculate the Lambert term for this fragment.
	 * This dot-product drives the rest of the lighting procedure.
	 */	
	float NdotL = clamp(dot(N, L), 0.0, 1.0);
	
	// The scene has an ambient term separate from any light
	vec4 ambient_global = gl_LightModel.ambient * gl_FrontMaterial.ambient * diffuse_map_color;
	
	// The ambient component from light 0
	vec4 ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient * diffuse_map_color;
	
	/* Calculate light fall-off according to the standard OpenGL light.
	 * attenuation function.
	 */
	float catt = gl_LightSource[0].constantAttenuation;
	float latt = gl_LightSource[0].linearAttenuation;
	float qatt = gl_LightSource[0].quadraticAttenuation;
	float d = length(vertex_to_light);
	float att = 1.0 / (catt + latt*d + qatt*d*d);
	
	// The diffuse component from light 0
	vec4 diffuse = 0.5 * gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse * diffuse_map_color * NdotL * att;

	// Calculate the specular component from light 0	
	vec4 specular = vec4(0);
	if(NdotL > 0.0) {
		vec3 reflected = reflect(-L, N);
		float spec = pow(max(0.0, dot(reflected, normalize(eye_to_vertex))), gl_FrontMaterial.shininess);
		specular = gl_FrontMaterial.specular * gl_LightSource[0].specular * spec * spec_multiplier;
	}

	// The final color is the linear combination of the colors computed above.
	gl_FragColor = ambient_global + ambient + diffuse + specular;
}

