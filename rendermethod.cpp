/** @file effect.cpp
 *    @brief The shader classes. Each shader should have a corresponding class
 *    in this file to communicate with OpenGL
 *    @author Zeyang Li (zeyangl)
 */
#include "glheaders.h"
#include "rendermethod.h"
#include "scene.h"
#include "imageio.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

bool app_is_glsl_enabled();

using namespace std;

#ifdef USE_GLSL
static char* load_file(const char* file)
{
    std::ifstream infile;
    infile.open(file);

    if(infile.fail()){
        fprintf(stderr, "ERROR: cannot open file %s\n", file);
        infile.close();
        exit(2);
    }

    infile.seekg(0, std::ios::end );
    int length = infile.tellg();
    infile.seekg(0, std::ios::beg );

    char* buffer = new char[length];
    infile.getline(buffer, length, '\0');

    infile.close();

    return buffer;
}

/**
 * Load a file as either a vertex shader or a fragment shader, and attach
 * it to a program 
 * @param file  The file to load
 * @param type  Either GL_VERTEX_SHADER_ARB, or GL_FRAGMENT_SHADER_ARB
 * @param program  The shading program to which the shaders are attached
 */
static void load_shader(const char* file, GLint type, GLhandleARB& program)
{    
    int result;
    char error_msg[1024];

     const char* src = load_file(file);
    // Create shader object
    GLhandleARB shader = glCreateShaderObjectARB(type);

    // Load Shader Sources
    glShaderSourceARB(shader, 1, &src, NULL);
    // Compile The Shaders
    glCompileShaderARB(shader);
    // Get compile result
    glGetObjectParameterivARB(shader, GL_OBJECT_COMPILE_STATUS_ARB, &result);
    if(!result){
        glGetInfoLogARB(shader, sizeof(error_msg), NULL, error_msg);
        fprintf(stderr, "GLSL COMPILE ERROR(%s): %s\n", file, error_msg);
        exit(2);
    }

    // Attach The Shader Objects To The Program Object
    glAttachObjectARB(program, shader);
}

/**
 * Creates a program, loads the given shader programs into it, and returns it.
 */  
static GLhandleARB load_shaders(const char* vert_file, const char* frag_file)
{
    // Create shader program
    GLhandleARB program  = glCreateProgramObjectARB();

    // Load vertex shader
    std::cout << "loading vertex shader " << vert_file << std::endl;
    load_shader(vert_file, GL_VERTEX_SHADER_ARB, program);

    // Load fragment shader
    std::cout << "loading fragment shader " << frag_file << std::endl;
    load_shader(frag_file, GL_FRAGMENT_SHADER_ARB, program);

    glLinkProgramARB(program);

    return program;
}
#endif /* USE_GLSL */

RenderMethod_DiffuseTexture::RenderMethod_DiffuseTexture(const Geometry *geom,
										   const Material *mat,
										   const Texture *diffuse_texture)
{
	assert(mat);
	assert(diffuse_texture);

	this->mat = mat;
	this->diffuse_texture = diffuse_texture;

	add_geom(geom);
}

void RenderMethod_DiffuseTexture::draw() const
{
	assert(mat);
	assert(diffuse_texture);

	mat->bind();

	// Disable texture unit 2
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	// Disable texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	// Bind texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuse_texture->get_gltex_name());
	glEnable(GL_TEXTURE_2D);

#ifdef USE_GLSL
	glUseProgramObjectARB(0);
#endif /* USE_GLSL */

	for(GeomList::const_iterator i=geoms.begin(); i!=geoms.end(); ++i)
	{
//		(*i)->set_transformation();
		(*i)->draw();
	}
}

RenderMethod_Fresnel::RenderMethod_Fresnel(const char* vert_file,
							 const char* frag_file,
							 const Geometry *geom,
							 const Material* mat,
							 const Texture* env_map)
{
	GLint env_map_uniform, n_t;

	assert(mat);
	assert(env_map);

	this->mat = mat;
	this->env_map = env_map;

	add_geom(geom);
	
#ifdef USE_GLSL
	program = load_shaders(vert_file, frag_file);

	// Set these uniforms only once when the effect is initialized
	glUseProgramObjectARB(program);
	
	env_map_uniform = glGetUniformLocationARB(program, "env_map");
	glUniform1iARB(env_map_uniform, 0);
	
	n_t = glGetUniformLocationARB(program, "n_t");
	glUniform1fARB(n_t, (GLfloat)mat->refraction_index);

	glUseProgramObjectARB(0);
#endif /* USE_GLSL */
}

void RenderMethod_Fresnel::draw() const
{
	assert(mat);
	assert(env_map);

	mat->bind();

	// Bind texture unit 2
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	// Bind texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	// Bind texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, env_map->get_gltex_name());
	glEnable(GL_TEXTURE_2D);

#ifdef USE_GLSL
	if(app_is_glsl_enabled()) {
		glUseProgramObjectARB(program);
	} else {
		glUseProgramObjectARB(0);
	}
#endif /* USE_GLSL */

	for(GeomList::const_iterator i=geoms.begin(); i!=geoms.end(); ++i)
	{
//		(*i)->set_transformation();
		(*i)->draw();
	}
}

RenderMethod_BumpMap::RenderMethod_BumpMap(const char* vert_file,
							 const char* frag_file,
							 const Geometry *geom,
							 const Material *mat,
							 const Texture *diffuse_map,
							 const Texture *normal_map,
							 const Texture *height_map)
{
	GLint diffuse_map_uniform, normal_map_uniform, height_map_uniform;

	assert(mat);
	assert(normal_map);
	assert(height_map);
	assert(diffuse_map);

	this->mat = mat;
	this->normal_map = normal_map;
	this->height_map = height_map;
	this->diffuse_map = diffuse_map;

	add_geom(geom);
		
#ifdef USE_GLSL
	program = load_shaders(vert_file, frag_file);

	// Set these uniforms only once when the effect is initialized
	
	glUseProgramObjectARB(program);
	
	diffuse_map_uniform = glGetUniformLocationARB(program, "diffuse_map");
	glUniform1iARB(diffuse_map_uniform, 0);
	
	normal_map_uniform = glGetUniformLocationARB(program, "normal_map");
	glUniform1iARB(normal_map_uniform, 1);
	
	height_map_uniform = glGetUniformLocationARB(program, "height_map");
	glUniform1iARB(height_map_uniform, 2);
	
	tangent_attrib_slot = glGetAttribLocationARB(program, "Tangent");
	
	glUseProgramObjectARB(0);

#endif /* USE_GLSL */
}

void RenderMethod_BumpMap::draw() const
{
	assert(mat);
	assert(normal_map);
	assert(height_map);
	assert(diffuse_map);

	mat->bind();

	// Bind texture unit 2
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, height_map->get_gltex_name());
	glEnable(GL_TEXTURE_2D);

	// Bind texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normal_map->get_gltex_name());
	glEnable(GL_TEXTURE_2D);

	// Bind texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuse_map->get_gltex_name());
	glEnable(GL_TEXTURE_2D);

#ifdef USE_GLSL
	if(app_is_glsl_enabled()) {
		glUseProgramObjectARB(program);
	} else {
		glUseProgramObjectARB(0);
	}
#endif /* USE_GLSL */

	for(GeomList::const_iterator i=geoms.begin(); i!=geoms.end(); ++i)
	{
//		(*i)->set_transformation();
		(*i)->draw();
	}
}