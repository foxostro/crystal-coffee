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
class Texture;

class Effect
{
public:
	Effect(void) : program(0) { /* Do Nothing */ }

    Effect(const char* vert_file, const char* frag_file);
    
    virtual ~Effect() { /* Do Nothing */ }
    
    virtual void bind(void) = 0;
    
    virtual bool areTangentsRequired() const { return false; }
    virtual GLint getTangentAttribSlot() const { return 0; }

protected:
    GLhandleARB program;
};

class DiffuseTextureEffect : public Effect
{
public:
	DiffuseTextureEffect(Material *mat, Texture *diffuse_texture);

	virtual void bind();

private:
	Material *mat;
	Texture *diffuse_texture;
};

class FresnelEffect : public Effect
{
public:
    FresnelEffect(const char* vert_file,
	              const char* frag_file,
				  const Material* mat,
                  const Texture* env_map);
                  
    virtual void bind();
    
protected:
	const Material* mat;
	const Texture* env_map;
};

class BumpMapEffect : public Effect
{
public:
    BumpMapEffect(const char *vert_file,
                  const char *frag_file,
				  Material *mat,
				  const Texture *diffuse_map,
                  const Texture *normal_map,
				  const Texture *height_map);
                  
    virtual void bind();
    
    virtual bool areTangentsRequired() const { return true; }
    virtual GLint getTangentAttribSlot() const { return tangent_attrib_slot; }
    
private:
	Material* mat;
	const Texture *normal_map;
	const Texture *height_map;
	const Texture *diffuse_map;
	GLint tangent_attrib_slot;
};

#endif /* _EFFECT_H_ */

