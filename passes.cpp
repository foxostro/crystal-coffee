#include <SDL/SDL.h>
#include "glheaders.h"
#include "scene.h"
#include "passes.h"

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
	set_light_positions(scene->lights); // light pos are fixed relative to the scene

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(RenderInstanceList::const_iterator i = instances.begin();
		i != instances.end(); ++i)
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		(*i)->draw();
		glPopAttrib();
		CHECK_GL_ERROR();
	}

	treelib_render();

	if(rendertarget) { glPopAttrib(); } // restore the viewport

	CHECK_GL_ERROR();
}

Mat4 CubeMapUpdatePass::face_orientation[6] =
{
	// GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT
	Mat4(1.0, 0.0, 0.0, 0.0,
	     0.0, 1.0, 0.0, 0.0,
		 0.0, 0.0, 1.0, 0.0,
		 0.0, 0.0, 0.0, 1.0),

	// GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT
	Mat4(1.0, 0.0, 0.0, 0.0,
	     0.0, 1.0, 0.0, 0.0,
		 0.0, 0.0, 1.0, 0.0,
		 0.0, 0.0, 0.0, 1.0),

	// GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT
	Mat4(0.0, 0.0, 0.0, 0.0,
	     0.0, 0.0, 0.0, 0.0,
		 0.0, 0.0, 0.0, 0.0,
		 0.0, 0.0, 0.0, 0.0),

	// GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT
	Mat4(0.0, 0.0, 0.0, 0.0,
	     0.0, 0.0, 0.0, 0.0,
		 0.0, 0.0, 0.0, 0.0,
		 0.0, 0.0, 0.0, 0.0),

	// GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT
	Mat4(0.0, 0.0, 0.0, 0.0,
	     0.0, 0.0, 0.0, 0.0,
		 0.0, 0.0, 0.0, 0.0,
		 0.0, 0.0, 0.0, 0.0),

	// GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT
	Mat4(0.0, 0.0, 0.0, 0.0,
	     0.0, 0.0, 0.0, 0.0,
		 0.0, 0.0, 0.0, 0.0,
		 0.0, 0.0, 0.0, 0.0),
};

void CubeMapUpdatePass::render(const Scene * scene)
{
	assert(scene);
	assert(rendertarget);

	CHECK_GL_ERROR();

	glClearColor((GLclampf)clear_color.x,
	             (GLclampf)clear_color.y,
				 (GLclampf)clear_color.z,
				 (GLclampf)clear_color.w);
	glClearDepth(1.0);

	glPushAttrib(GL_VIEWPORT_BIT); // save the viewport

	for(int i=0; i<6; ++i)
	{
		rendertarget->bind_render_target(i);

		// set the projection matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glLoadMatrixd(proj.m);

		// set the modelview matrix for this face
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslated(-camera.position.x, -camera.position.y, -camera.position.z);
		glMultMatrixd(face_orientation[i].m);

		set_light_positions(scene->lights);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for(RenderInstanceList::const_iterator i = instances.begin();
			i != instances.end(); ++i)
		{
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			(*i)->draw();
			glPopAttrib();
			CHECK_GL_ERROR();
		}

		treelib_render();
	}

	glPopAttrib(); // restore the viewport

	CHECK_GL_ERROR();
}
