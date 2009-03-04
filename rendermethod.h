/** @file effect.h
 *  @brief Define the interface to shaders
 *
 *  @author Zeyang Li (zeyangl)
 *  @author Eric Butler (edbutler)
 *  @author Andrew Fox (arfox)
 */

#ifndef _EFFECT_H_
#define _EFFECT_H_

#include "glheaders.h"
#include "vec/vec.h"
#include "vec/mat.h"
#include <string>

template<class TYPE> class BufferObject;
class Material;
class Texture;
class ShaderProgram;

class RenderMethod
{
public:
	virtual ~RenderMethod() { /* Do Nothing */ }
	virtual void draw(const Mat4 &transform) const = 0;

protected:
	RenderMethod(void) { /* Do Nothing */ }
};

class RenderMethod_DiffuseTexture : public RenderMethod
{
public:
	RenderMethod_DiffuseTexture(const BufferObject<Vec3> * vertices_buffer,
                                const BufferObject<Vec3> * normals_buffer,
								const BufferObject<Vec2> * tcoords_buffer,
								const BufferObject<index_t> * indices_buffer,
                                const Material * mat,
	                            const Texture * diffuse_texture);

	virtual void draw(const Mat4 &transform) const;

private:
	const BufferObject<Vec3> * vertices_buffer;
	const BufferObject<Vec3> * normals_buffer;
	const BufferObject<Vec2> * tcoords_buffer;
	const BufferObject<index_t> * indices_buffer;
	const Material *mat;
	const Texture *diffuse_texture;
};

class RenderMethod_Fresnel : public RenderMethod
{
public:
    RenderMethod_Fresnel(const BufferObject<Vec3> * vertices_buffer,
                         const BufferObject<Vec3> * normals_buffer,
                         const BufferObject<index_t> * indices_buffer,
						 const ShaderProgram * shader,
				         const Material * mat,
				         const Texture * env_map,
				         real_t refraction_index);

	virtual void draw(const Mat4 &transform) const;
    
private:
	const BufferObject<Vec3> * vertices_buffer;
	const BufferObject<Vec3> * normals_buffer;
	const BufferObject<index_t> * indices_buffer;
	const ShaderProgram * shader;
	const Material* mat;
	const Texture* env_map;
};

class RenderMethod_BumpMap : public RenderMethod
{
public:
    RenderMethod_BumpMap(const BufferObject<Vec3> * vertices_buffer,
                         const BufferObject<Vec3> * normals_buffer,
                         const BufferObject<Vec4> * tangents_buffer,
						 const BufferObject<Vec2> * tcoords_buffer,
						 const BufferObject<index_t> * indices_buffer,
						 const ShaderProgram * shader,
				         const Material * mat,
				         const Texture * diffuse_map,
                         const Texture * normal_map,
				         const Texture * height_map);

	virtual void draw(const Mat4 &transform) const;
    
private:
	GLint tangent_attrib_slot;
	
	const BufferObject<Vec3> * vertices_buffer;
	const BufferObject<Vec3> * normals_buffer;
	const BufferObject<Vec4> * tangents_buffer;
	const BufferObject<Vec2> * tcoords_buffer;
	const BufferObject<index_t> * indices_buffer;
	const ShaderProgram * shader;
	const Material * mat;
	const Texture * normal_map;
	const Texture * height_map;
	const Texture * diffuse_map;
};

#endif /* _EFFECT_H_ */

