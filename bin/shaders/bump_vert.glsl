attribute vec4 Tangent;

varying vec3 vertex_to_light;
varying vec3 eye_to_vertex;

void main()
{
	vec3 vertex_in_eye_space;
	
	// First, get some standard computations out of the way
	gl_Position = ftransform();           // Transform vertex position
	gl_TexCoord[0] = gl_MultiTexCoord0;   // Send texcoords to the frag shader

	// transform the vertex position into eye-space
	vertex_in_eye_space = vec3(gl_ModelViewMatrix * gl_Vertex);

	// vector from the vertex to the light (in eye-space)
	vertex_to_light = vec3(gl_LightSource[0].position) - vertex_in_eye_space;
	
	// vector from the vertex to the eye (in eye-space)
	eye_to_vertex = -vec3(vertex_in_eye_space);
	
	// TBN will transform from eye-space to tangent-space
	vec3 n = vec3(gl_NormalMatrix * gl_Normal);
	vec3 t = vec3(gl_NormalMatrix * Tangent.xyz);
	vec3 b = cross(n, t) * Tangent.w;
	mat3 to_tangent_space = mat3(vec3(t.x, b.x, n.x),
	                             vec3(t.y, b.y, n.y),
	                             vec3(t.z, b.z, n.z)); // transpose(mat3(t, b, n));
		
	// Transform these to tangent-space for the fragment shader
	vertex_to_light = to_tangent_space * vertex_to_light;
	eye_to_vertex = to_tangent_space * eye_to_vertex;
}

