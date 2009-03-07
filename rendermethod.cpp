/** @file effect.cpp
 *  @brief The shader classes. Each shader should have a corresponding class
 *  in this file to communicate with OpenGL
 *  @author Zeyang Li (zeyangl)
 *  @author Andrew Fox (arfox)
 */

#include "glheaders.h"
#include "rendermethod.h"
#include "scene.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

RenderMethod_DiffuseTexture::
RenderMethod_DiffuseTexture(const BufferObject<Vec3> * vertices_buffer,
                            const BufferObject<Vec3> * normals_buffer,
							const BufferObject<Vec2> * tcoords_buffer,
							const BufferObject<index_t> * indices_buffer,
                            const Material * mat,
	                        const Texture * diffuse_texture)
{
	assert(vertices_buffer);
	assert(normals_buffer);
	assert(tcoords_buffer);
	assert(mat);
	assert(diffuse_texture);

	this->vertices_buffer = vertices_buffer;
	this->normals_buffer = normals_buffer;
	this->tcoords_buffer = tcoords_buffer;
	this->indices_buffer = indices_buffer;
	this->mat = mat;
	this->diffuse_texture = diffuse_texture;
}

void RenderMethod_DiffuseTexture::draw(const Mat4 &transform) const
{	
	assert(vertices_buffer);
	assert(normals_buffer);
	assert(tcoords_buffer);
	assert(mat);
	assert(diffuse_texture);

	mat->bind();

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

	glMultMatrixd(transform.m);
	
	// Bind the vertex buffer
	glEnableClientState(GL_VERTEX_ARRAY);
	vertices_buffer->bind();
	glVertexPointer(3, GL_DOUBLE, 0, 0);
	
	// Bind the normals buffer
	glEnableClientState(GL_NORMAL_ARRAY);
	normals_buffer->bind();
	glNormalPointer(GL_DOUBLE, 0, 0);

	// Bind the tcoord buffer
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	tcoords_buffer->bind();
	glTexCoordPointer(2, GL_DOUBLE, 0, 0);

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

RenderMethod_TextureReplace::
RenderMethod_TextureReplace(const BufferObject<Vec3> * vertices_buffer,
							const BufferObject<Vec3> * normals_buffer,
							const BufferObject<Vec2> * tcoords_buffer,
							const BufferObject<index_t> * indices_buffer,
							const Texture * diffuse_texture)
{
	assert(vertices_buffer);
	assert(normals_buffer);
	assert(tcoords_buffer);
	assert(diffuse_texture);

	this->vertices_buffer = vertices_buffer;
	this->normals_buffer = normals_buffer;
	this->tcoords_buffer = tcoords_buffer;
	this->indices_buffer = indices_buffer;
	this->diffuse_texture = diffuse_texture;
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

	glMultMatrixd(transform.m);

	// Bind the vertex buffer
	glEnableClientState(GL_VERTEX_ARRAY);
	vertices_buffer->bind();
	glVertexPointer(3, GL_DOUBLE, 0, 0);

	// Bind the normals buffer
	glEnableClientState(GL_NORMAL_ARRAY);
	normals_buffer->bind();
	glNormalPointer(GL_DOUBLE, 0, 0);

	// Bind the tcoord buffer
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	tcoords_buffer->bind();
	glTexCoordPointer(2, GL_DOUBLE, 0, 0);

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

RenderMethod_FresnelSphereMap::
RenderMethod_FresnelSphereMap(const BufferObject<Vec3> * vertices_buffer,
					          const BufferObject<Vec3> * normals_buffer,
					          const BufferObject<index_t> * indices_buffer,
					          const ShaderProgram * shader,
				              const Material * mat,
				              const Texture * env_map,
				              real_t refraction_index)
{
	GLint env_map_uniform, n_t;

	assert(vertices_buffer);
	assert(normals_buffer);
	assert(shader);
	assert(mat);
	assert(env_map);

	this->vertices_buffer = vertices_buffer;
	this->normals_buffer = normals_buffer;
	this->indices_buffer = indices_buffer;
	this->shader = shader;
	this->mat = mat;
	this->env_map = env_map;

	GLhandleARB program = shader->get_program();

	// Set these uniforms only once when the effect is initialized
	glUseProgramObjectARB(program);
	
	env_map_uniform = glGetUniformLocationARB(program, "env_map");
	glUniform1iARB(env_map_uniform, 0);
	
	n_t = glGetUniformLocationARB(program, "n_t");
	glUniform1fARB(n_t, (GLfloat)refraction_index);

	glUseProgramObjectARB(0);
}

void RenderMethod_FresnelSphereMap::draw(const Mat4 &transform) const
{
	assert(vertices_buffer);
	assert(normals_buffer);
	assert(shader);
	assert(mat);
	assert(env_map);

	mat->bind();
	GLhandleARB program = shader->get_program();

	// Disable texture unit 2
	glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_2D);

	// Disable texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);

	// Bind texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, env_map->get_gltex_name());
	glEnable(GL_TEXTURE_2D);

	glUseProgramObjectARB(program);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glMultMatrixd(transform.m);
	
	// Bind the vertex buffer
	glEnableClientState(GL_VERTEX_ARRAY);
	vertices_buffer->bind();
	glVertexPointer(3, GL_DOUBLE, 0, 0);
	
	// Bind the normals buffer
	glEnableClientState(GL_NORMAL_ARRAY);
	normals_buffer->bind();
	glNormalPointer(GL_DOUBLE, 0, 0);

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
}

RenderMethod_Fresnel::
RenderMethod_Fresnel(const BufferObject<Vec3> * vertices_buffer,
					 const BufferObject<Vec3> * normals_buffer,
					 const BufferObject<Vec2> * tcoords_buffer,
					 const BufferObject<index_t> * indices_buffer,
					 const ShaderProgram * shader,
				     const Material * mat,
				     const Texture * diffuse_map,
				     real_t refraction_index)
{
	GLint diffuse_map_uniform, n_t;

	assert(vertices_buffer);
	assert(normals_buffer);
	assert(tcoords_buffer);
	assert(shader);
	assert(mat);
	assert(diffuse_map);

	this->vertices_buffer = vertices_buffer;
	this->normals_buffer = normals_buffer;
	this->tcoords_buffer = tcoords_buffer;
	this->indices_buffer = indices_buffer;
	this->shader = shader;
	this->mat = mat;
	this->diffuse_map = diffuse_map;

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
	assert(mat);
	assert(env_map);

	mat->bind();
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

	glMultMatrixd(transform.m);
	
	// Bind the vertex buffer
	glEnableClientState(GL_VERTEX_ARRAY);
	vertices_buffer->bind();
	glVertexPointer(3, GL_DOUBLE, 0, 0);
	
	// Bind the normals buffer
	glEnableClientState(GL_NORMAL_ARRAY);
	normals_buffer->bind();
	glNormalPointer(GL_DOUBLE, 0, 0);

	// Bind the normals buffer
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	tcoords_buffer->bind();
	glTexCoordPointer(2, GL_DOUBLE, 0, 0);

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
RenderMethod_BumpMap(const BufferObject<Vec3> * vertices_buffer,
                     const BufferObject<Vec3> * normals_buffer,
                     const BufferObject<Vec4> * tangents_buffer,
					 const BufferObject<Vec2> * tcoords_buffer,
					 const BufferObject<index_t> * indices_buffer,
					 const ShaderProgram * shader,
				     const Material * mat,
				     const Texture * diffuse_map,
                     const Texture * normal_map,
				     const Texture * height_map)
{
	GLint diffuse_map_uniform, normal_map_uniform, height_map_uniform;

	assert(vertices_buffer);
	assert(normals_buffer);
	assert(tangents_buffer);
	assert(tcoords_buffer);
	assert(shader);
	assert(mat);
	assert(normal_map);
	assert(height_map);
	assert(diffuse_map);

	this->vertices_buffer = vertices_buffer;
	this->normals_buffer = normals_buffer;
	this->tangents_buffer = tangents_buffer;
	this->tcoords_buffer = tcoords_buffer;
	this->indices_buffer = indices_buffer;
	this->shader = shader;
	this->mat = mat;
	this->normal_map = normal_map;
	this->height_map = height_map;
	this->diffuse_map = diffuse_map;

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
	assert(mat);
	assert(normal_map);
	assert(height_map);
	assert(diffuse_map);

	mat->bind();
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

	glMultMatrixd(transform.m);
	
	// Bind the vertex buffer
	glEnableClientState(GL_VERTEX_ARRAY);
	vertices_buffer->bind();
	glVertexPointer(3, GL_DOUBLE, 0, 0);
	
	// Bind the normals buffer
	glEnableClientState(GL_NORMAL_ARRAY);
	normals_buffer->bind();
	glNormalPointer(GL_DOUBLE, 0, 0);
	
	// Bind the tangents buffer
	glEnableVertexAttribArrayARB(tangent_attrib_slot);
	tangents_buffer->bind();
	glVertexAttribPointerARB(tangent_attrib_slot, 4,
		                     GL_DOUBLE, GL_FALSE, 0, 0);

	// Bind the tcoord buffer
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	tcoords_buffer->bind();
	glTexCoordPointer(2, GL_DOUBLE, 0, 0);

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

