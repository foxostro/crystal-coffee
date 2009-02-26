uniform sampler2D env_map;

void main (void)
{
	vec4 env = texture2D(env_map, gl_TexCoord[0].st);

	gl_FragColor = vec4(env.xyz, 1.0);
}

