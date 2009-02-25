varying vec3 vertex_to_light;
varying vec3 eye_to_vertex;

/* Calculates the tangent vector for a vertex.

Perpendicular to the vertex normal is a plane of all possible tangent vectors.

For the best results, it is possible to perform the tangent calculation on the
CPU, taking into account the topology of each face to ensure tangents are
defined consistently. This is precisely how were advised to handle tangents
for this project, and I believe this is necessary for more advanced shading
effects.

However, there are at least two other techniques that are sufficient for simple
bump-mapping:

1) Texture uv make up a consistent and smoothly interpolatable coordinate system
at each vertex. The tangent vector may be constrained to lie within the plane
of either u or v.

2) For most meshes, it doesn't matter whether the tangents are garaunteed to be
defined consistently across the entire mesh. We can constrain the tangent vector
to lie within some arbitrary plane, and so long as we are consistent about which
plane we choose. This is the technique demonstrated below.

Source: <http://www.gamedev.net/community/forums/topic.asp?topic_id=252198>
*/
vec3 calc_tangent()
{
	vec3 tangent;
	
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
	
	return tangent;
}

void main()
{
	vec3 vertex_in_eye_space, v;
	
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
	vec3 t = vec3(gl_NormalMatrix * calc_tangent());
	vec3 b = cross(n, t);
	mat3 to_tangent_space = mat3(t, b, n);
	
	// Transform these to tangent-space for the fragment shader
	vertex_to_light = to_tangent_space * vertex_to_light;
	eye_to_vertex = to_tangent_space * eye_to_vertex;
}

