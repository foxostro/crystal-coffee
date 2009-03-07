varying vec3 vertex_to_eye;
varying vec3 normal;

void main()
{	
	vec3 eye, r;
	
	gl_Position = ftransform(); // Standard transformation to clip-coords

	// Get the reflected eye-vector (eye-space)
	eye = normalize(vec3(gl_ModelViewMatrix * gl_Vertex));
	normal = normalize(gl_NormalMatrix * gl_Normal);
	r = reflect(eye, normal);
	
    // Map the uv according to the sphere-mapping eqns on the handout
	float p = 2.0 * sqrt(r.x*r.x + r.y*r.y + (r.z+1.0)*(r.z+1.0));
	gl_TexCoord[0].s = r.x/p + 0.5;
	gl_TexCoord[0].t = r.y/p + 0.5;
	
	// vector from the vertex to the eye (in eye-space)
	vertex_to_eye = -eye;
}

