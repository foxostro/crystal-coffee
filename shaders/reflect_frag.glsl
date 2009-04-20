uniform samplerCube CubeMap;
varying vec3 refdir;

void main (void)
{
    gl_FragColor = textureCube(CubeMap, refdir);
}
