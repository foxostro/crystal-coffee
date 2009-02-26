attribute vec4 Tangent;

varying vec3 vertex_to_light;
varying vec3 eye_to_vertex;

void main()
{
	vec3 vertex_in_eye_space, eye, r, normal;
	
	// First, get some standard computations out of the way
	gl_Position = ftransform();           // Transform vertex position
	gl_TexCoord[0] = gl_MultiTexCoord0;   // Send texcoords to the frag shader
	
	// Get the reflected eye-vector (eye-space)
	eye = normalize(vec3(gl_ModelViewMatrix * gl_Vertex));
	normal = normalize(gl_NormalMatrix * gl_Normal);
	r = reflect(eye, normal);
	
	// Map the uv according to the sphere-mapping eqns on the handout
	float p = 2.0 * sqrt(r.x*r.x + r.y*r.y + (r.z+1.0)*(r.z+1.0));
	gl_TexCoord[1].s = r.x/p + 0.5;
	gl_TexCoord[1].t = r.y/p + 0.5;

	// transform the vertex position into eye-space
	vertex_in_eye_space = vec3(gl_ModelViewMatrix * gl_Vertex);

	// vector from the vertex to the light (in eye-space)
	vertex_to_light = vec3(gl_LightSource[0].position) - vertex_in_eye_space;
	
	// vector from the vertex to the eye (in eye-space)
	eye_to_vertex = -vec3(vertex_in_eye_space);
	
	// TBN will transform from eye-space to tangent-space
	vec3 n = normal;
	vec3 t = vec3(gl_NormalMatrix * Tangent.xyz);
	vec3 b = cross(n, t) * Tangent.w;
	mat3 to_tangent_space = mat3(vec3(t.x, b.x, n.x),
	                             vec3(t.y, b.y, n.y),
	                             vec3(t.z, b.z, n.z)); // transpose(mat3(t, b, n));
	
	// Transform these to tangent-space for the fragment shader
	vertex_to_light = to_tangent_space * vertex_to_light;
	eye_to_vertex = to_tangent_space * eye_to_vertex;
}

