#include <SDL/SDL.h>

#include "glheaders.h"
#include "scene.h"
#include "passes.h"

#include <iostream>

void StandardPass::render(const Scene * scene)
{
	assert(scene);

	CHECK_GL_ERROR();

	glClearColor((GLclampf)clear_color.x,
	             (GLclampf)clear_color.y,
				 (GLclampf)clear_color.z,
				 (GLclampf)clear_color.w);
	glClearDepth(1.0);


	if(rendertarget) {
		glPushAttrib(GL_VIEWPORT_BIT); // save the viewport (set by RenderTarget)
		rendertarget->bind_render_target();
	} else {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

	set_camera();
	set_light_positions(scene->lights); // light eye are fixed relative to the scene

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(RenderInstanceList::const_iterator i = instances.begin();
		i != instances.end(); ++i)
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		(*i)->draw();
		glPopAttrib();
		CHECK_GL_ERROR();
	}

	if(rendertarget) { glPopAttrib(); } // restore the viewport

	CHECK_GL_ERROR();
}

CubeMapUpdatePass::CubeMapUpdatePass()
{
	dimensions = ivec2(128, 128);
	rt = boost::shared_ptr<RenderTarget2D>(new RenderTarget2D(dimensions));
}

void CubeMapUpdatePass::render(const Scene * scene)
{
	assert(scene);
	assert(cubemaptarget);

	CHECK_GL_ERROR();

	glClearColor((GLclampf)clear_color.x,
	             (GLclampf)clear_color.y,
				 (GLclampf)clear_color.z,
				 (GLclampf)clear_color.w);
	glClearDepth(1.0);

	glPushAttrib(GL_VIEWPORT_BIT); // save the viewport

	glMatrixMode(GL_PROJECTION); // save the projection matrix
	glPushMatrix();

	for(int i=0; i<6; ++i)
	{
		cubemaptarget->bind_render_target(i);

		glMatrixMode(GL_MODELVIEW); // save the modelview matrix
		glPushMatrix();
		set_camera(camera.get_position(), face_orientation[i]);

		set_light_positions(scene->lights);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		CHECK_GL_ERROR();
		for(RenderInstanceList::const_iterator i = instances.begin();
			i != instances.end(); ++i)
		{
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			(*i)->draw();
			glPopAttrib();
			CHECK_GL_ERROR();
		}

		glPopMatrix(); // restore the modelview matrix
	}

	glMatrixMode(GL_PROJECTION); // restore the projection matrix
	glPopMatrix();

	glPopAttrib(); // restore the viewport

	CHECK_GL_ERROR();
}

void CubeMapUpdatePass::set_camera(const Vec3 &eye, const Quat &orientation)
{
	const Vec3 up = orientation * Vec3::UnitY;
	const Vec3 direction = orientation * -Vec3::UnitZ;
	const GLdouble centerx = eye.x + direction.x;
	const GLdouble centery = eye.y + direction.y;
	const GLdouble centerz = eye.z + direction.z;

	// set the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixr(proj.m);

	// set the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.x, eye.y, eye.z,
	          centerx, centery, centerz,
	          up.x, up.y, up.z);
}
