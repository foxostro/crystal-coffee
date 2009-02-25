/** @file effect.h
 *  @brief Define the interface to shaders
 *
 *  @author Zeyang Li (zeyangl)
 *  @author Eric Butler (edbutler)
 */
#ifndef _EFFECT_H_
#define _EFFECT_H_

#include "glheaders.h"
#include "vec/vec.h"
#include <string>

class Material;

class EnvironmentMap
{
public:
    virtual ~EnvironmentMap() {}
    virtual void load_texture() = 0;
    virtual Vec3 get_texture_color(const Vec3& direction) const = 0;
};

class SphereMap : public EnvironmentMap
{
public:
    SphereMap();
    ~SphereMap();
    virtual void load_texture();
    virtual Vec3 get_texture_color(const Vec3& direction) const;

public:
    std::string texture_name;
    GLuint gltex_name;
    unsigned char* texture;
    int tex_width, tex_height;
};


/* DO NOT change the signature of the constructors 
   defined in *Effect classes. The staff scene needs them
   to compile. You may change the signatures of any other functions
   if desired.
 */

/* STUDENT SOLUTION STARTS HERE
   TODO P2 add members and code to implement shaders. 
 */
class Effect
{
public:
    Effect(const char* vert_file, const char* frag_file);
    
    virtual ~Effect() {}
    
    virtual void bind(void) = 0;

protected:
    GLhandleARB program;
};

class FresnelEffect : public Effect
{
public:
    FresnelEffect(const char* vert_file, const char* frag_file,
                  const SphereMap* env_map, Material* mat);
                  
    virtual void bind();
};

class BumpMapEffect : public Effect
{
public:
    BumpMapEffect(const char* vert_file, const char* frag_file,
                  Material* diffuse_mat, Material* normal_mat);
                  
    virtual void bind();
    
private:
	Material* diffuse_mat;
	Material* normal_mat;
};

#endif /* _EFFECT_H_ */

