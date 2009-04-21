uniform mat4 wld_space_to_obj_space;
varying vec3 refdir_in_wld_space;

void main()
{
	vec3 vertex_in_wld_space, normal_in_wld_space;
	
	// transform vertex position into clip-space
	gl_Position = ftransform();
	
	normal_in_wld_space = vec4(wld_space_to_obj_space * vec4(gl_Normal, 1.0)).xyz;
	vertex_in_wld_space = vec4(wld_space_to_obj_space * gl_Vertex).xyz;
	refdir_in_wld_space = reflect(vertex_in_wld_space, normal_in_wld_space);
}
