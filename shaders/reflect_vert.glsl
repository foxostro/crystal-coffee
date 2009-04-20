varying vec3 refdir;

void main(void)
{
   vec4 camcen= gl_ModelViewMatrixInverse[3];//camera center in World Space Coordinates
   vec4 dir   = gl_Vertex - camcen;//(Assuming) gl_Vertex is given in WSC
   refdir     = reflect(vec3(dir.x, dir.y, dir.z), gl_Normal);
   gl_Position = ftransform();
}
