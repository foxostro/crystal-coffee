#include <SDL/SDL.h>
#include "glheaders.h"
#include "scene.h"
#include "passes.h"

void StandardPass::render(const Scene * scene)
{
	assert(scene);

	set_camera();
	set_light_positions(scene->lights); // light pos are fixed relative to the scene

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(RenderInstanceList::const_iterator i = instances.begin();
		i != instances.end(); ++i)
	{
		(*i)->draw();
	}

	treelib_render();

	// flush and swap the buffer
	glFlush();
	SDL_GL_SwapBuffers();
}
