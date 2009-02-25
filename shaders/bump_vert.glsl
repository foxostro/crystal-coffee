varying vec3 vertex_to_light;
varying vec3 eye_to_vertex;

void main()
{
	vec3 vertex_in_eye_space, v;
	
	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;

	// transform the vertex position into eye-space
	vertex_in_eye_space = vec3(gl_ModelViewMatrix * gl_Vertex);

	// vector from the vertex to the light (in eye-space)
	vertex_to_light = vec3(gl_LightSource[0].position) - vertex_in_eye_space;
	
	// vector from the vertex to the eye (in eye-space)
	eye_to_vertex = -vec3(vertex_in_eye_space);
	
	// calculate the tangent vector
	vec3 tangent; 
	vec3 binormal; 
	vec3 c1 = cross(gl_Normal, vec3(0.0, 0.0, 1.0)); 
	vec3 c2 = cross(gl_Normal, vec3(0.0, 1.0, 0.0)); 
	 
	if(length(c1) > length(c2))
	{
		tangent = c1;
	}
	else
	{
		tangent = c2;
	}
	 
	tangent = normalize(tangent);
	 
	binormal = cross(gl_Normal, tangent); 
	binormal = normalize(binormal);
	
	// TBN will transform from eye-space to tangent-space
	vec3 n = vec3(gl_NormalMatrix * gl_Normal);
	vec3 t = vec3(gl_NormalMatrix * tangent);
	vec3 b = cross(n, t);
	mat3 to_tangent_space = mat3(t, b, n);
	
	// Transform these to tangent-space for the fragment shader
	vertex_to_light = to_tangent_space * vertex_to_light;
	eye_to_vertex = to_tangent_space * eye_to_vertex;
}

