varying vec3 vertex_to_eye;
varying vec3 normal;

void main()
{
	gl_Position = ftransform(); // Standard transformation to clip-coords
    gl_TexCoord[0] = gl_MultiTexCoord0;
	
	// vector from the vertex to the eye (in eye-space)
	vertex_to_eye = -normalize(vec3(gl_ModelViewMatrix * gl_Vertex));
}
