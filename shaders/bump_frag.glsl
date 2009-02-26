uniform sampler2D diffuse_map;
uniform sampler2D normal_map;
uniform sampler2D height_map;

varying vec3 vertex_to_light;
varying vec3 eye_to_vertex;

// Tweak the height taken from the parallax heightmap
const float parallax_height_scale = 0.03;

vec2 parallax_tcoord(vec2 tcoord, vec3 E)
{
	float depth = texture2D(height_map, tcoord).x;
	vec2 half_offset = E.xy * depth * parallax_height_scale;

	depth = (depth + texture2D(height_map, tcoord + half_offset).x) * 0.5;
	half_offset = E.xy * depth * parallax_height_scale;

	depth = (depth + texture2D(height_map, tcoord + half_offset).x) * 0.5;
	half_offset = E.xy * depth * parallax_height_scale;

	tcoord = tcoord + half_offset;
	
	return tcoord;
}

void main()
{
	vec3 E = normalize(eye_to_vertex);

	// Get the texture coordinates to use, applying a "parallax" effect
	// NOTE: We are scaling the texture coordinates here to tile the tex more.
	vec2 tcoord = parallax_tcoord(gl_TexCoord[0].st, E);
	
	// Get the color of the diffuse texture at this fragment
	vec4 diffuse_map_color = texture2D(diffuse_map, tcoord);
	
	/* Get the perturbed normal from the normal map.
	 * This normal can be used in the Lambert term almost directly. We need to
	 * unpack it from the texture, though.
	 */
	vec3 N = texture2D(normal_map, tcoord).xyz * 2.0 - 1.0;
	
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
	vec4 diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse * diffuse_map_color * NdotL * att;

	// Calculate the specular component from light 0	
	vec4 specular = vec4(0);
	if(NdotL > 0.0) {
		vec3 reflected = reflect(-L, N);
		float spec = pow(max(0.0, dot(reflected, normalize(eye_to_vertex))), gl_FrontMaterial.shininess);
		specular = gl_FrontMaterial.specular * gl_LightSource[0].specular * spec;
	}

	// The final color is the linear combination of the colors computed above.
	gl_FragColor = ambient_global + ambient + diffuse + specular;
}

