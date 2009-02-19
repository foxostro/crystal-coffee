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

    // TODO P2 load the sphere map as OpenGL texture object
}

SphereMap::~SphereMap()
{
    // free the loaded texture, will have no effect if null
    free(texture);
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
{
}

FresnelEffect::FresnelEffect(const char* vert_file, const char* frag_file,
                             const SphereMap* env_map, Material* mat)
    : Effect(vert_file, frag_file)
{
    // TODO P2 create shader program object for fresnel effect.
}

BumpMapEffect::BumpMapEffect(const char* vert_file, const char* frag_file,
                             Material* diffuse, Material* normal)
    : Effect(vert_file, frag_file) 
{
    // TODO P2 create shader program object for bump map effect.
}
 

