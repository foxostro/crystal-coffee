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
#include <vector>

class Geometry;
class Material;
class Texture;

class RenderMethod
{
public:
    virtual ~RenderMethod() { /* Do Nothing */ }
    
	virtual void draw(void) const = 0;

	void add_geom(const Geometry * geom)
	{
		if(geom) {
			geoms.push_back(geom);
		}
	}

protected:
	RenderMethod(void) { /* Do Nothing */ }

	typedef std::vector<const Geometry *> GeomList;
	GeomList geoms;
};

class RenderMethod_DiffuseTexture : public RenderMethod
{
public:
	RenderMethod_DiffuseTexture(const Geometry *geom,
	                     const Material *mat,
	                     const Texture *diffuse_texture);

	virtual void draw() const;

private:
	const Material *mat;
	const Texture *diffuse_texture;
};

class RenderMethod_Fresnel : public RenderMethod
{
public:
    RenderMethod_Fresnel(const char* vert_file,
	              const char* frag_file,
				  const Geometry *geom,
				  const Material* mat,
				  const Texture* env_map,
				  real_t refraction_index);

	virtual void draw() const;
    
protected:
	GLhandleARB program;
	const Material* mat;
	const Texture* env_map;
};

class RenderMethod_BumpMap : public RenderMethod
{
public:
    RenderMethod_BumpMap(const char *vert_file,
                  const char *frag_file,
				  const Geometry *geom,
				  const Material *mat,
				  const Texture *diffuse_map,
                  const Texture *normal_map,
				  const Texture *height_map);

	virtual void draw() const;
    
private:
	GLhandleARB program;
	const Material* mat;
	const Texture *normal_map;
	const Texture *height_map;
	const Texture *diffuse_map;
	GLint tangent_attrib_slot;
};

#endif /* _EFFECT_H_ */

