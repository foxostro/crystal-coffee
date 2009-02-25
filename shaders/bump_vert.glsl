varying vec3 vertex_to_light;
varying vec3 eye_to_vertex;

attribute vec3 tangent;

void main()
{
	vec3 vertex_in_eye_space, v;
	
	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;

	// transform the vertex position into eye-space
	vertex_in_eye_space = vec3(gl_ModelViewMatrix * gl_Vertex);

	// vector from the vertex to the light (in eye-space)
	vertex_to_light = vec3(gl_LightSource[0].position - vertex_in_eye_space);
	
	// vector from the vertex to the eye (in eye-space)
	eye_to_vertex = -vec3(vertex_in_eye_space);
	
	// TBN will transform from eye-space to tangent-space
	vec3 n = normalize(gl_NormalMatrix * gl_Normal);
	vec3 t = vec3(gl_NormalMatrix * tangent);
	vec3 b = cross(n, t);
	mat3 to_tangent_space = mat3(t, b, n);
	
	// Transform these to tangent-space for the fragment shader
	vertex_to_light = to_tangent_space * vertex_to_light;
	eye_to_vertex = to_tangent_space * eye_to_vertex;
}

