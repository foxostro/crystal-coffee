varying vec3 normal, vp;
varying float d;

void main()
{	
	/* Transform the normal into eye-clip space and renormalize */
	normal = normalize(gl_NormalMatrix * gl_Normal);
	
	/* Transform the vertex into eye-clip coordinates as well */
	gl_Position = ftransform();
	
	/* Get a vector from the light position to the eye (in eye-clip space). */
	vp = vec3(gl_LightSource[0].position - gl_Position);
	
	/* Calculate the distance from the light to the eye. */
	d = length(vp);

	/* Normalize to get the direction from the light to the eye. */
	vp = normalize(vp);
	
	/* interpolate tex-coords across the face */
	gl_TexCoord[0] = gl_MultiTexCoord0;
}

