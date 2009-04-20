/**
 * @file project.cpp
 * @brief The rendering/update code.
 *
 * @author Andrew Fox (arfox)
 */

#include "vec/mat.h"
#include "glheaders.h"
#include "project.h"
#include "scene.h"
#include "treelib.h"
#include "geom/sphere.h"
#include <iostream>
using namespace std;

#define PERIOD (0.01)

// current absolute simulation time for the current scene
static real_t sim_time;

static void init_light_properties(const LightList & lights);

static void draw(RenderInstance *o)
{
	assert(o);
	o->draw();
}

static void init_resource(boost::shared_ptr<SceneResource> resource)
{
	assert(resource);
	resource->init();
	CHECK_GL_ERROR();
}

/**
 * Initializes all state for the given scene.
 * @param scene The next scene that will be drawn.
 */
void prj_initialize( Scene* scene )
{
	assert(scene);

	CHECK_GL_ERROR();
	
    // reset scene time
    sim_time = scene->start_time;

	GLfloat lmodel_ambient[] = { (GLfloat)scene->ambient_light.x,
	                             (GLfloat)scene->ambient_light.y,
	                             (GLfloat)scene->ambient_light.z, 1 };
	glClearColor(0, 0, 0, 1);
	glShadeModel(GL_SMOOTH);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_CULL_FACE);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    // Initialize scene resources
	for_each(scene->resources.begin(),
	         scene->resources.end(),
	         &init_resource);
	         
	init_light_properties(scene->lights);

	CHECK_GL_ERROR();
}

/**
 * Updates the scene world by stepping forward by the given time.
 * @param scene The scene to update.
 * @param delta_time The time step in seconds.
 * @remark The caller will invoke glutPostRedisplay after update, so there
 *  is no need to invoke it here.
 */
void prj_update(Scene* scene, double delta_time)
{
	assert(scene);
	
    // increment time and update all updatable geometries
    sim_time += PERIOD;
    Scene::TickableList& list = scene->tickables;
    for (Scene::TickableList::iterator i = list.begin();
            i != list.end(); ++i)
        (*i)->tick(sim_time);
}

static void init_light_properties(const LightList & lights)
{
	const GLfloat catt = 1;
	const GLfloat latt = 0;
	const GLfloat qatt = 0;
	
	const GLfloat black[] = { 0, 0, 0, 1 };
	const GLfloat white[] = { 1, 1, 1, 1 };

	for(int i=0; i<8; ++i) {
		glDisable(GL_LIGHT0 + i);
	}

	for(int i=0; i<8 && i < (int)lights.size(); ++i)
	{
		const Light & light = lights[i];

		const GLfloat color[] = { (GLfloat)light.color.x,
		                          (GLfloat)light.color.y,
								  (GLfloat)light.color.z, 1 };

		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, black);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, color);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, white);

		glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION,  catt * (GLfloat)light.intensity);
		glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION,    latt);
		glLightf(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, qatt);

		glEnable(GL_LIGHT0 + i);
	}
}
