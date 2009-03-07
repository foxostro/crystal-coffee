#include <SDL/SDL.h>
#include "glheaders.h"
#include "scene.h"
#include "passes.h"

void StandardPass::render(const Scene * scene)
{
	assert(scene);
	assert(rendertarget);

	CHECK_GL_ERROR();

	glClearColor((GLclampf)clear_color.x,
	             (GLclampf)clear_color.y,
				 (GLclampf)clear_color.z,
				 (GLclampf)clear_color.w);

	glPushAttrib(GL_VIEWPORT_BIT); // save the viewport (set by RenderTarget)
	rendertarget->bind();

	set_camera();
	set_light_positions(scene->lights); // light pos are fixed relative to the scene

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(RenderInstanceList::const_iterator i = instances.begin();
		i != instances.end(); ++i)
	{
		CHECK_GL_ERROR();
		(*i)->draw();
		CHECK_GL_ERROR();
	}

	treelib_render();

	rendertarget->unbind();
	glPopAttrib(); // restore the viewport (altered by RenderTarget)
}
