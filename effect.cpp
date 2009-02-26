/** @file effect.cpp
 *    @brief The shader classes. Each shader should have a corresponding class
 *    in this file to communicate with OpenGL
 *    @author Zeyang Li (zeyangl)
 */
#include "glheaders.h"
#include "effect.h"
#include "scene.h"
#include "imageio.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

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


SphereMap::SphereMap():
     gltex_name(0), texture(0), tex_width(0), tex_height(0) {}

void SphereMap::load_texture()
{
    if (texture)
        return;

    texture = imageio_load_image(texture_name.c_str(), &tex_width, &tex_height);
	if(texture == NULL)
		std::cout << "sphere map " << texture_name << "load failed\n";

	glGenTextures(1, &gltex_name);
	glBindTexture(GL_TEXTURE_2D, gltex_name);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA,
	             GL_UNSIGNED_BYTE, texture);
    
}

SphereMap::~SphereMap()
{
    // free the loaded texture, will have no effect if null
    free(texture);
    
    glDeleteTextures(1, &gltex_name);
}

Vec3 SphereMap::get_texture_color(const Vec3& direction) const
{
    if (!texture)
        return Vec3::Zero;

    int tx, ty;

    Vec3 n = direction.unit();
    n.z += 1.0;

    if (n.z == 0) {
        tx = 0;
        ty = tex_height / 2;
    } else {
        real_t p = 2*n.magnitude();
        Vec2 tc(n.x / p + .5, n.y / p + .5);

        // nearest sampling
        tx = static_cast<int>(tc.x * (tex_width - 1));
        ty = static_cast<int>(tc.y * (tex_height - 1));
    }

    assert(0 <= tx && 0 <= ty && tx < tex_width && ty < tex_height);

    return color_array_to_vector(texture + 4 * (tx + ty * tex_width)).xyz();
}


Effect::Effect(const char* vert_file, const char* frag_file)
	: program(0)
{
	program = load_shaders(vert_file, frag_file);
}

FresnelEffect::FresnelEffect(const char *vert_file, const char *frag_file,
                             const SphereMap *env, Material *_mat)
    : Effect(vert_file, frag_file),
      mat(_mat),
	  env_mat(env)
{
	GLint diffuse_map, env_map;
		
	// Set texture sampler uniforms only once
	glUseProgramObjectARB(program);
	env_map = glGetUniformLocationARB(program, "env_map");
	glUniform1iARB(env_map, 0);
	glUseProgramObjectARB(0);
}

void FresnelEffect::bind()
{
	// Bind texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	
	// Bind texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, env_mat->gltex_name);
	glEnable(GL_TEXTURE_2D);
			
	glUseProgramObjectARB(program);
}

BumpMapEffect::BumpMapEffect(const char* vert_file, const char* frag_file,
                             Material* diffuse, Material* normal)
    : Effect(vert_file, frag_file),
      diffuse_mat(diffuse),
      normal_mat(normal)
{
	GLint diffuse_map, normal_map;
		
	// Set texture sampler uniforms only once
	glUseProgramObjectARB(program);
	diffuse_map = glGetUniformLocationARB(program, "diffuse_map");
	glUniform1iARB(diffuse_map, 0);
	normal_map = glGetUniformLocationARB(program, "normal_map");
	glUniform1iARB(normal_map, 1);
	glUseProgramObjectARB(0);
	
	tangent_attrib_slot = glGetAttribLocation(program, "Tangent");
}

void BumpMapEffect::bind(void)
{
	// Bind texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normal_mat->gltex_name);
	glEnable(GL_TEXTURE_2D);
	
	// Bind texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuse_mat->gltex_name);
	glEnable(GL_TEXTURE_2D);
			
	glUseProgramObjectARB(program);
}

