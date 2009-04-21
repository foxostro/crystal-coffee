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
#include "material.h"
#include <string>
#include <boost/shared_ptr.hpp>

template<class TYPE> class BufferObject;
class Material;
class Texture;
class ShaderProgram;
class CubeMapTexture;

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
	RenderMethod_DiffuseTexture(const boost::shared_ptr< const BufferObject<Vec3> > vertices_buffer,
                                const boost::shared_ptr< const BufferObject<Vec3> > normals_buffer,
								const boost::shared_ptr< const BufferObject<Vec2> > tcoords_buffer,
								const boost::shared_ptr< const BufferObject<index_t> > indices_buffer,
                                const Material & mat,
	                            const boost::shared_ptr<const Texture> diffuse_texture);

	virtual void draw(const Mat4 &transform) const;

private:
	const boost::shared_ptr< const BufferObject<Vec3> > vertices_buffer;
	const boost::shared_ptr< const BufferObject<Vec3> > normals_buffer;
	const boost::shared_ptr< const BufferObject<Vec2> > tcoords_buffer;
	const boost::shared_ptr< const BufferObject<index_t> > indices_buffer;
	const Material mat;
	const boost::shared_ptr<const Texture> diffuse_texture;
};

class RenderMethod_TextureReplace : public RenderMethod
{
public:
	RenderMethod_TextureReplace(const boost::shared_ptr< const BufferObject<Vec3> > vertices_buffer,
                                const boost::shared_ptr< const BufferObject<Vec3> > normals_buffer,
								const boost::shared_ptr< const BufferObject<Vec2> > tcoords_buffer,
								const boost::shared_ptr< const BufferObject<index_t> > indices_buffer,
	                            boost::shared_ptr<const Texture> diffuse_texture);

	virtual void draw(const Mat4 &transform) const;

private:
	const boost::shared_ptr< const BufferObject<Vec3> > vertices_buffer;
	const boost::shared_ptr< const BufferObject<Vec3> > normals_buffer;
	const boost::shared_ptr< const BufferObject<Vec2> > tcoords_buffer;
	const boost::shared_ptr< const BufferObject<index_t> > indices_buffer;
	const boost::shared_ptr<const Texture> diffuse_texture;
};

class RenderMethod_FresnelEnvMap : public RenderMethod
{
public:
    RenderMethod_FresnelEnvMap(const Mat4 &wld_space_to_obj_space,
	                           const boost::shared_ptr< const BufferObject<Vec3> > vertices_buffer,
                               const boost::shared_ptr< const BufferObject<Vec3> > normals_buffer,
                               const boost::shared_ptr< const BufferObject<index_t> > indices_buffer,
						       const boost::shared_ptr<const ShaderProgram> shader,
				               const Material & mat,
				               const boost::shared_ptr<const Texture> env_map,
				               real_t refraction_index);

	virtual void draw(const Mat4 &transform) const;
    
private:
	const Mat4 wld_space_to_obj_space;
	GLint wld_space_to_obj_space_uniform;

	const boost::shared_ptr< const BufferObject<Vec3> > vertices_buffer;
	const boost::shared_ptr< const BufferObject<Vec3> > normals_buffer;
	const boost::shared_ptr< const BufferObject<index_t> > indices_buffer;
	const boost::shared_ptr<const ShaderProgram> shader;
	const Material mat;
	const boost::shared_ptr<const Texture> env_map;
};

class RenderMethod_Fresnel : public RenderMethod
{
public:
	RenderMethod_Fresnel(const boost::shared_ptr< const BufferObject<Vec3> > vertices_buffer,
	                     const boost::shared_ptr< const BufferObject<Vec3> > normals_buffer,
	                     const boost::shared_ptr< const BufferObject<Vec2> > tcoords_buffer,
	                     const boost::shared_ptr< const BufferObject<index_t> > indices_buffer,
	                     boost::shared_ptr<const ShaderProgram> shader,
	                     const Material & mat,
	                     boost::shared_ptr<const Texture> diffuse_map,
	                     real_t refraction_index);

	virtual void draw(const Mat4 &transform) const;

private:
	const boost::shared_ptr< const BufferObject<Vec3> > vertices_buffer;
	const boost::shared_ptr< const BufferObject<Vec3> > normals_buffer;
	const boost::shared_ptr< const BufferObject<Vec2> > tcoords_buffer;
	const boost::shared_ptr< const BufferObject<index_t> > indices_buffer;
	const boost::shared_ptr<const ShaderProgram> shader;
	const Material mat;
	const boost::shared_ptr<const Texture> diffuse_map;
};

class RenderMethod_BumpMap : public RenderMethod
{
public:
    RenderMethod_BumpMap(const boost::shared_ptr< const BufferObject<Vec3> > vertices_buffer,
                         const boost::shared_ptr< const BufferObject<Vec3> > normals_buffer,
                         const boost::shared_ptr< const BufferObject<Vec4> > tangents_buffer,
						 const boost::shared_ptr< const BufferObject<Vec2> > tcoords_buffer,
						 const boost::shared_ptr< const BufferObject<index_t> > indices_buffer,
						 const boost::shared_ptr<const ShaderProgram> shader,
				         const Material & mat,
				         const boost::shared_ptr<const Texture> diffuse_map,
                         const boost::shared_ptr<const Texture> normal_map,
				         const boost::shared_ptr<const Texture> height_map);

	virtual void draw(const Mat4 &transform) const;
    
private:
	GLint tangent_attrib_slot;
	
	const boost::shared_ptr< const BufferObject<Vec3> > vertices_buffer;
	const boost::shared_ptr< const BufferObject<Vec3> > normals_buffer;
	const boost::shared_ptr< const BufferObject<Vec4> > tangents_buffer;
	const boost::shared_ptr< const BufferObject<Vec2> > tcoords_buffer;
	const boost::shared_ptr< const BufferObject<index_t> > indices_buffer;
	const boost::shared_ptr<const ShaderProgram> shader;
	const Material mat;
	const boost::shared_ptr<const Texture> normal_map;
	const boost::shared_ptr<const Texture> height_map;
	const boost::shared_ptr<const Texture> diffuse_map;
};

class RenderMethod_CubemapReflection : public RenderMethod
{
public:
	RenderMethod_CubemapReflection(const Mat4 &wld_space_to_obj_space,
	                               const boost::shared_ptr< const BufferObject<Vec3> > _vertices_buffer,
                                   const boost::shared_ptr< const BufferObject<Vec3> > _normals_buffer,
								   const boost::shared_ptr< const BufferObject<index_t> > _indices_buffer,
                                   const Material & _mat,
								   const boost::shared_ptr<const CubeMapTexture> _cubemap,
								   const boost::shared_ptr<const ShaderProgram> _shader);

	virtual void draw(const Mat4 &transform) const;

private:
	const Mat4 wld_space_to_obj_space;
	GLint wld_space_to_obj_space_uniform;

	const boost::shared_ptr< const BufferObject<Vec3> > vertices_buffer;
	const boost::shared_ptr< const BufferObject<Vec3> > normals_buffer;
	const boost::shared_ptr< const BufferObject<index_t> > indices_buffer;
	const Material mat;
	const boost::shared_ptr<const CubeMapTexture> cubemap;
	const boost::shared_ptr<const ShaderProgram> shader;
};

#endif /* _EFFECT_H_ */
