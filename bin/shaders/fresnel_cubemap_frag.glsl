uniform samplerCube env_map;
uniform float n_t; // Refractive Index (we assume the "air" has n = 1.0)

varying vec3 refdir_in_wld_space;
varying vec3 vertex_to_eye;
varying vec3 normal;

void main()
{	
	// Get the color of the environment cubemap at this fragment
	vec4 c_e = textureCube(env_map, refdir_in_wld_space);

	// Renormalize the interpolated normal (eye-space, btw)
	vec3 N = normalize(normal);
	
	// Renormalize the interpolated vector to the eye (eye-space, btw)
	vec3 D = normalize(vertex_to_eye);
	
	vec4 c_d = gl_FrontMaterial.diffuse;

	// Calculate the Fresnel term
	float sqrt_R_0 = (n_t - 1.0) / (n_t + 1.0);
	float R_0 = sqrt_R_0 * sqrt_R_0;
	float R = R_0 + (1.0 - R_0) * pow(1.0 - dot(D, N), 2.0);
	
	// The final color is the linear combination of the colors computed above.
	gl_FragColor = c_e*R + c_d*(1.0-R);
}
