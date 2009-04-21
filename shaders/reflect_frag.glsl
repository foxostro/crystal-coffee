uniform samplerCube env_map;
varying vec3 refdir_in_wld_space;

void main()
{
	gl_FragColor = textureCube(env_map, refdir_in_wld_space);
}
