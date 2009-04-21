uniform mat4 wld_space_to_obj_space;
varying vec3 refdir_in_wld_space;
varying vec3 vertex_to_eye;
varying vec3 normal;

void main()
{	
	vec3 vertex_in_eye_space, normal_in_eye_space;
	vec3 vertex_in_wld_space, normal_in_wld_space;
	
////////////////////////////////////////////////////////////	
	
	normal_in_wld_space = vec4(wld_space_to_obj_space * vec4(gl_Normal, 1.0)).xyz;
	vertex_in_wld_space = vec4(wld_space_to_obj_space * gl_Vertex).xyz;
	refdir_in_wld_space = reflect(vertex_in_wld_space, normal_in_wld_space);
	
////////////////////////////////////////////////////////////

	normal_in_eye_space = normalize(gl_NormalMatrix * gl_Normal);
    vertex_in_eye_space = vec4(gl_ModelViewMatrix * gl_Vertex).xyz;
	vertex_to_eye = -vertex_in_eye_space;
	normal = normal_in_eye_space;
	
////////////////////////////////////////////////////////////

	gl_Position = ftransform();
}
