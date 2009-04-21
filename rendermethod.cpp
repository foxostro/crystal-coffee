/** @file effect.cpp
 *  @brief The shader classes. Each shader should have a corresponding class
 *  in this file to communicate with OpenGL
 *  @author Zeyang Li (zeyangl)
 *  @author Andrew Fox (arfox)
 */

#include "glheaders.h"
#include "rendermethod.h"
#include "scene.h"
#include "vec/mat.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

RenderMethod_DiffuseTexture::
RenderMethod_DiffuseTexture(const boost::shared_ptr< const BufferObject<Vec3> > _vertices_buffer,
                            const boost::shared_ptr< const BufferObject<Vec3> > _normals_buffer,
							const boost::shared_ptr< const BufferObject<Vec2> > _tcoords_buffer,
							const boost::shared_ptr< const BufferObject<index_t> > _indices_buffer,
                            const Material & _mat,
	                        const boost::shared_ptr< const Texture > _diffuse_texture)
: vertices_buffer(_vertices_buffer),
  normals_buffer(_normals_buffer),
  tcoords_buffer(_tcoords_buffer),
  indices_buffer(_indices_buffer),
  mat(_mat),
  diffuse_texture(_diffuse_texture)
{
	assert(vertices_buffer);
	assert(normals_buffer);
	assert(diffuse_texture);
}

void RenderMethod_DiffuseTexture::draw(const Mat4 &transform) const
{	
	assert(vertices_buffer);
	assert(normals_buffer);
	assert(diffuse_texture);

	CHECK_GL_ERROR();

	mat.bind();

	// Disable all texture units
	int num_tex_units=1;
	glGetIntegerv(GL_MAX_TEXTURE_COORDS, &num_tex_units);
	for(int i=num_tex_units-1; i>=0; --i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_3D);
		glDisable(GL_TEXTURE_CUBE_MAP_EXT);
	}

	// Bind texture unit 0
	glActiveTexture(GL_TEXTURE0);
	diffuse_texture->bind();

	glUseProgramObjectARB(0); // fixed-function pipeline
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glMultMatrixr(transform.m);
	
	// Bind the vertex buffer
	glEnableClientState(GL_VERTEX_ARRAY);
	vertices_buffer->bind();
#if REAL_IS_DOUBLE
	glVertexPointer(3, GL_DOUBLE, 0, 0);
#else
	glVertexPointer(3, GL_FLOAT, 0, 0);
#endif
	
	// Bind the normals buffer
	glEnableClientState(GL_NORMAL_ARRAY);
	normals_buffer->bind();
#if REAL_IS_DOUBLE
	glNormalPointer(GL_DOUBLE, 0, 0);
#else
	glNormalPointer(GL_FLOAT, 0, 0);
#endif

	// Bind the tcoord buffer
	if(tcoords_buffer) {
		glClientActiveTexture(GL_TEXTURE0);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		tcoords_buffer->bind();
		#if REAL_IS_DOUBLE
				glTexCoordPointer(2, GL_DOUBLE, 0, 0);
		#else
				glTexCoordPointer(2, GL_FLOAT, 0, 0);
		#endif
	}

	// Actually draw the triangles	
	if(indices_buffer) {
		// Draw using the index buffer
		GLsizei count = indices_buffer->getNumber();
		indices_buffer->bind();
		glDrawElements(GL_TRIANGLES, count, MESH_INDEX_FORMAT, 0);
	} else {
		glDrawArrays(GL_TRIANGLES, 0, vertices_buffer->getNumber());
	}

	// Clean up
	if(tcoords_buffer) { glDisableClientState(GL_TEXTURE_COORD_ARRAY); }
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();

	CHECK_GL_ERROR();
}

RenderMethod_TextureReplace::
RenderMethod_TextureReplace(const boost::shared_ptr< const BufferObject<Vec3> > _vertices_buffer,
							const boost::shared_ptr< const BufferObject<Vec3> > _normals_buffer,
							const boost::shared_ptr< const BufferObject<Vec2> > _tcoords_buffer,
							const boost::shared_ptr< const BufferObject<index_t> > _indices_buffer,
							const boost::shared_ptr< const Texture > _diffuse_texture)
: vertices_buffer(_vertices_buffer),
  normals_buffer(_normals_buffer),
  tcoords_buffer(_tcoords_buffer),
  indices_buffer(_indices_buffer),
  diffuse_texture(_diffuse_texture)
{
	assert(vertices_buffer);
	assert(normals_buffer);
	assert(tcoords_buffer);
	assert(diffuse_texture);
}

void RenderMethod_TextureReplace::draw(const Mat4 &transform) const
{	
	assert(vertices_buffer);
	assert(normals_buffer);
	assert(tcoords_buffer);
	assert(diffuse_texture);

	glDisable(GL_LIGHTING);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// Disable texture unit 2
	glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_2D);

	// Disable texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);

	// Bind texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuse_texture->get_gltex_name());
	glEnable(GL_TEXTURE_2D);

	glUseProgramObjectARB(0); // fixed-function pipeline

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glMultMatrixr(transform.m);

	// Bind the vertex buffer
	glEnableClientState(GL_VERTEX_ARRAY);
	vertices_buffer->bind();
#if REAL_IS_DOUBLE
	glVertexPointer(3, GL_DOUBLE, 0, 0);
#else
	glVertexPointer(3, GL_FLOAT, 0, 0);
#endif

	// Bind the normals buffer
	glEnableClientState(GL_NORMAL_ARRAY);
	normals_buffer->bind();
#if REAL_IS_DOUBLE
	glNormalPointer(GL_DOUBLE, 0, 0);
#else
	glNormalPointer(GL_FLOAT, 0, 0);
#endif

	// Bind the tcoord buffer
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	tcoords_buffer->bind();
#if REAL_IS_DOUBLE
	glTexCoordPointer(2, GL_DOUBLE, 0, 0);
#else
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
#endif

	// Actually draw the triangles	
	if(indices_buffer) {
		// Draw using the index buffer
		GLsizei count = indices_buffer->getNumber();
		indices_buffer->bind();
		glDrawElements(GL_TRIANGLES, count, MESH_INDEX_FORMAT, 0);
	} else {
		glDrawArrays(GL_TRIANGLES, 0, vertices_buffer->getNumber());
	}

	// Clean up
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();

	glEnable(GL_LIGHTING);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

RenderMethod_FresnelEnvMap::
RenderMethod_FresnelEnvMap(const Mat4 &_wld_space_to_obj_space,
						   const boost::shared_ptr< const BufferObject<Vec3> > _vertices_buffer,
					       const boost::shared_ptr< const BufferObject<Vec3> > _normals_buffer,
					       const boost::shared_ptr< const BufferObject<index_t> > _indices_buffer,
					       const boost::shared_ptr< const ShaderProgram > _shader,
						   const Material & _mat,
						   const boost::shared_ptr<const Texture> _env_map,
				           real_t refraction_index)
: wld_space_to_obj_space(_wld_space_to_obj_space),
  wld_space_to_obj_space_uniform(0),
  vertices_buffer(_vertices_buffer),
  normals_buffer(_normals_buffer),
  indices_buffer(_indices_buffer),
  shader(_shader),
  mat(_mat),
  env_map(_env_map)
{
	GLint env_map_uniform, n_t;

	assert(vertices_buffer);
	assert(normals_buffer);
	assert(shader);
	assert(env_map);

	GLhandleARB program = shader->get_program();

	glUseProgramObjectARB(program);

	// Set these uniforms whenever the object is rendered
	wld_space_to_obj_space_uniform = glGetUniformLocationARB(program, "wld_space_to_obj_space");
	
	// Set these uniforms only once when the effect is initialized
	env_map_uniform = glGetUniformLocationARB(program, "env_map");
	glUniform1iARB(env_map_uniform, 0);

	n_t = glGetUniformLocationARB(program, "n_t");
	glUniform1fARB(n_t, (GLfloat)refraction_index);

	glUseProgramObjectARB(0);
}

void RenderMethod_FresnelEnvMap::draw(const Mat4 &transform) const
{
	assert(vertices_buffer);
	assert(normals_buffer);
	assert(shader);
	assert(env_map);

	CHECK_GL_ERROR();

	mat.bind();

	// Disable texture unit 2
	glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_2D);

	// Disable texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);

	// Bind texture unit 0
	glActiveTexture(GL_TEXTURE0);
	env_map->bind();
	CHECK_GL_ERROR();

	// Bind the shader program
	glUseProgramObjectARB(shader->get_program());
#if REAL_IS_DOUBLE
#pragma error("There is no glUniformMatrix4dv function. Manual conversion is necessary!")
#else
	glUniformMatrix4fv(wld_space_to_obj_space_uniform, 16, GL_FALSE, wld_space_to_obj_space.m);
#endif
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glMultMatrixr(transform.m);
	
	// Bind the vertex buffer
	glEnableClientState(GL_VERTEX_ARRAY);
	vertices_buffer->bind();
#if REAL_IS_DOUBLE
	glVertexPointer(3, GL_DOUBLE, 0, 0);
#else
	glVertexPointer(3, GL_FLOAT, 0, 0);
#endif
	
	// Bind the normals buffer
	glEnableClientState(GL_NORMAL_ARRAY);
	normals_buffer->bind();
#if REAL_IS_DOUBLE
	glNormalPointer(GL_DOUBLE, 0, 0);
#else
	glNormalPointer(GL_FLOAT, 0, 0);
#endif

	// Actually draw the triangles	
	if(indices_buffer) {
		// Draw using the index buffer
		GLsizei count = indices_buffer->getNumber();
		indices_buffer->bind();
		glDrawElements(GL_TRIANGLES, count, MESH_INDEX_FORMAT, 0);
	} else {
		glDrawArrays(GL_TRIANGLES, 0, vertices_buffer->getNumber());
	}
	
	// Clean up
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();

	CHECK_GL_ERROR();
}

RenderMethod_Fresnel::
RenderMethod_Fresnel(const boost::shared_ptr< const BufferObject<Vec3> > _vertices_buffer,
					 const boost::shared_ptr< const BufferObject<Vec3> > _normals_buffer,
					 const boost::shared_ptr< const BufferObject<Vec2> > _tcoords_buffer,
					 const boost::shared_ptr< const BufferObject<index_t> > _indices_buffer,
					 const boost::shared_ptr< const ShaderProgram > _shader,
				     const Material & _mat,
				     const boost::shared_ptr< const Texture > _diffuse_map,
				     real_t refraction_index)
: vertices_buffer(_vertices_buffer),
  normals_buffer(_normals_buffer),
  tcoords_buffer(_tcoords_buffer),
  indices_buffer(_indices_buffer),
  shader(_shader),
  mat(_mat),
  diffuse_map(_diffuse_map)
{
	GLint diffuse_map_uniform, n_t;

	assert(vertices_buffer);
	assert(normals_buffer);
	assert(tcoords_buffer);
	assert(shader);
	assert(diffuse_map);

	GLhandleARB program = shader->get_program();

	// Set these uniforms only once when the effect is initialized
	glUseProgramObjectARB(program);
	
	diffuse_map_uniform = glGetUniformLocationARB(program, "diffuse_map");
	glUniform1iARB(diffuse_map_uniform, 0);
	
	n_t = glGetUniformLocationARB(program, "n_t");
	glUniform1fARB(n_t, (GLfloat)refraction_index);

	glUseProgramObjectARB(0);
}

void RenderMethod_Fresnel::draw(const Mat4 &transform) const
{
	assert(vertices_buffer);
	assert(normals_buffer);
	assert(shader);
	assert(diffuse_map);

	mat.bind();
	GLhandleARB program = shader->get_program();

	// Disable texture unit 2
	glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_2D);

	// Disable texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);

	// Bind texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuse_map->get_gltex_name());
	glEnable(GL_TEXTURE_2D);

	glUseProgramObjectARB(program);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glMultMatrixr(transform.m);
	
	// Bind the vertex buffer
	glEnableClientState(GL_VERTEX_ARRAY);
	vertices_buffer->bind();
#if REAL_IS_DOUBLE
	glVertexPointer(3, GL_DOUBLE, 0, 0);
#else
	glVertexPointer(3, GL_FLOAT, 0, 0);
#endif
	
	// Bind the normals buffer
	glEnableClientState(GL_NORMAL_ARRAY);
	normals_buffer->bind();
#if REAL_IS_DOUBLE
	glNormalPointer(GL_DOUBLE, 0, 0);
#else
	glNormalPointer(GL_FLOAT, 0, 0);
#endif

	// Bind the normals buffer
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	tcoords_buffer->bind();
#if REAL_IS_DOUBLE
	glTexCoordPointer(2, GL_DOUBLE, 0, 0);
#else
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
#endif

	// Actually draw the triangles	
	if(indices_buffer) {
		// Draw using the index buffer
		GLsizei count = indices_buffer->getNumber();
		indices_buffer->bind();
		glDrawElements(GL_TRIANGLES, count, MESH_INDEX_FORMAT, 0);
	} else {
		glDrawArrays(GL_TRIANGLES, 0, vertices_buffer->getNumber());
	}
	
	// Clean up
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
}

RenderMethod_BumpMap::
RenderMethod_BumpMap(const boost::shared_ptr< const BufferObject<Vec3> > _vertices_buffer,
                     const boost::shared_ptr< const BufferObject<Vec3> > _normals_buffer,
                     const boost::shared_ptr< const BufferObject<Vec4> > _tangents_buffer,
					 const boost::shared_ptr< const BufferObject<Vec2> > _tcoords_buffer,
					 const boost::shared_ptr< const BufferObject<index_t> > _indices_buffer,
					 const boost::shared_ptr< const ShaderProgram > _shader,
				     const Material & _mat,
				     const boost::shared_ptr< const Texture > _diffuse_map,
                     const boost::shared_ptr< const Texture > _normal_map,
				     const boost::shared_ptr< const Texture > _height_map)
: vertices_buffer(_vertices_buffer),
  normals_buffer(_normals_buffer),
  tangents_buffer(_tangents_buffer),
  tcoords_buffer(_tcoords_buffer),
  indices_buffer(_indices_buffer),
  shader(_shader),
  mat(_mat),
  normal_map(_normal_map),
  height_map(_height_map),
  diffuse_map(_diffuse_map)
{
	GLint diffuse_map_uniform, normal_map_uniform, height_map_uniform;

	assert(vertices_buffer);
	assert(normals_buffer);
	assert(tangents_buffer);
	assert(tcoords_buffer);
	assert(shader);
	assert(normal_map);
	assert(height_map);
	assert(diffuse_map);

	// Set these uniforms only once when the effect is initialized

	GLhandleARB program = shader->get_program();
	
	glUseProgramObjectARB(program);
	
	diffuse_map_uniform = glGetUniformLocationARB(program, "diffuse_map");
	glUniform1iARB(diffuse_map_uniform, 0);
	
	normal_map_uniform = glGetUniformLocationARB(program, "normal_map");
	glUniform1iARB(normal_map_uniform, 1);
	
	height_map_uniform = glGetUniformLocationARB(program, "height_map");
	glUniform1iARB(height_map_uniform, 2);
	
	tangent_attrib_slot = glGetAttribLocationARB(program, "Tangent");
	
	glUseProgramObjectARB(0);
}

void RenderMethod_BumpMap::draw(const Mat4 &transform) const
{
	assert(vertices_buffer);
	assert(normals_buffer);
	assert(tangents_buffer);
	assert(tcoords_buffer);
	assert(normal_map);
	assert(height_map);
	assert(diffuse_map);

	mat.bind();
	GLhandleARB program = shader->get_program();

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

	glUseProgramObjectARB(program);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glMultMatrixr(transform.m);
	
	// Bind the vertex buffer
	glEnableClientState(GL_VERTEX_ARRAY);
	vertices_buffer->bind();
#if REAL_IS_DOUBLE
	glVertexPointer(3, GL_DOUBLE, 0, 0);
#else
	glVertexPointer(3, GL_FLOAT, 0, 0);
#endif
	
	// Bind the normals buffer
	glEnableClientState(GL_NORMAL_ARRAY);
	normals_buffer->bind();
#if REAL_IS_DOUBLE
	glNormalPointer(GL_DOUBLE, 0, 0);
#else
	glNormalPointer(GL_FLOAT, 0, 0);
#endif
	
	// Bind the tangents buffer
	glEnableVertexAttribArrayARB(tangent_attrib_slot);
	tangents_buffer->bind();

#if REAL_IS_DOUBLE
	glVertexAttribPointerARB(tangent_attrib_slot, 4, GL_DOUBLE, GL_FALSE, 0, 0);
#else
	glVertexAttribPointerARB(tangent_attrib_slot, 4, GL_FLOAT, GL_FALSE, 0, 0);
#endif

	// Bind the tcoord buffer
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	tcoords_buffer->bind();

#if REAL_IS_DOUBLE
	glTexCoordPointer(2, GL_DOUBLE, 0, 0);
#else
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
#endif

	// Actually draw the triangles	
	if(indices_buffer) {
		// Draw using the index buffer
		GLsizei count = indices_buffer->getNumber();
		indices_buffer->bind();
		glDrawElements(GL_TRIANGLES, count, MESH_INDEX_FORMAT, 0);
	} else {
		glDrawArrays(GL_TRIANGLES, 0, vertices_buffer->getNumber());
	}

	// Clean up
	glDisableVertexAttribArrayARB(tangent_attrib_slot);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
}

RenderMethod_CubemapReflection::
RenderMethod_CubemapReflection(const Mat4 &_wld_space_to_obj_space,
							   const boost::shared_ptr< const BufferObject<Vec3> > _vertices_buffer,
							   const boost::shared_ptr< const BufferObject<Vec3> > _normals_buffer,
							   const boost::shared_ptr< const BufferObject<index_t> > _indices_buffer,
							   const Material & _mat,
							   const boost::shared_ptr<const CubeMapTexture> _cubemap,
							   const boost::shared_ptr<const ShaderProgram> _shader)
: wld_space_to_obj_space(_wld_space_to_obj_space),
  vertices_buffer(_vertices_buffer),
  normals_buffer(_normals_buffer),
  indices_buffer(_indices_buffer),
  mat(_mat),
  cubemap(_cubemap),
  shader(_shader)
{
	GLint CubeMap_uniform;

	assert(vertices_buffer);
	assert(normals_buffer);
	assert(cubemap);
	assert(shader);

	const GLhandleARB program = shader->get_program();

	glUseProgramObjectARB(program);

	CubeMap_uniform = glGetUniformLocationARB(program, "CubeMap");
	glUniform1iARB(CubeMap_uniform, 0);

	wld_space_to_obj_space_uniform = glGetUniformLocationARB(program, "wld_space_to_obj_space");

	glUseProgramObjectARB(0);
}

void RenderMethod_CubemapReflection::draw(const Mat4 &transform) const
{
	assert(vertices_buffer);
	assert(normals_buffer);
	assert(cubemap);
	assert(shader);

	CHECK_GL_ERROR();

	mat.bind();

	// Disable all texture units
	int num_tex_units=1;
	glGetIntegerv(GL_MAX_TEXTURE_COORDS, &num_tex_units);
	for(int i=num_tex_units-1; i>=0; --i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_3D);
		glDisable(GL_TEXTURE_CUBE_MAP_EXT);
	}

	// Bind texture unit 0
	glActiveTexture(GL_TEXTURE0);
	cubemap->bind();

	// Bind the shader program
	glUseProgram(shader->get_program());
#if REAL_IS_DOUBLE
#pragma error("There is no glUniformMatrix4dv function. Manual conversion is necessary!")
#else
	glUniformMatrix4fv(wld_space_to_obj_space_uniform, 16, GL_FALSE, wld_space_to_obj_space.m);
#endif

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glMultMatrixr(transform.m);

	// Bind the vertex buffer
	glEnableClientState(GL_VERTEX_ARRAY);
	vertices_buffer->bind();
#if REAL_IS_DOUBLE
	glVertexPointer(3, GL_DOUBLE, 0, 0);
#else
	glVertexPointer(3, GL_FLOAT, 0, 0);
#endif

	// Bind the normals buffer
	glEnableClientState(GL_NORMAL_ARRAY);
	normals_buffer->bind();
#if REAL_IS_DOUBLE
	glNormalPointer(GL_DOUBLE, 0, 0);
#else
	glNormalPointer(GL_FLOAT, 0, 0);
#endif

	// Actually draw the triangles	
	if(indices_buffer) {
		// Draw using the index buffer
		GLsizei count = indices_buffer->getNumber();
		indices_buffer->bind();
		glDrawElements(GL_TRIANGLES, count, MESH_INDEX_FORMAT, 0);
	} else {
		glDrawArrays(GL_TRIANGLES, 0, vertices_buffer->getNumber());
	}

	// Clean up
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();

	CHECK_GL_ERROR();
}
