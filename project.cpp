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

static int num_gl_lights=8;

static void set_light_positions(const Scene::LightList & lights);
static void init_light_properties(const Scene::LightList & lights);

static void draw(RenderInstance *o)
{
	assert(o);
	o->draw();
}

static void init_resource(SceneResource * resource)
{
	assert(resource);
	resource->init();
}

/**
 * Initializes all state for the given scene.
 * @param scene The next scene that will be drawn.
 */
void prj_initialize( Scene* scene )
{
	assert(scene);
	
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
	glGetIntegerv(GL_MAX_LIGHTS, &num_gl_lights);
    
    // Initialize scene resources
	for_each(scene->resources.begin(),
	         scene->resources.end(),
	         &init_resource);
	         
	init_light_properties(scene->lights);
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

static void set_camera(const Camera &camera) {
	real_t fov = camera.get_fov_degrees();
	real_t aspect = camera.get_aspect_ratio();
	real_t near_clip = camera.get_near_clip();
	real_t far_clip = camera.get_far_clip();
	real_t eyex = camera.get_position().x;
	real_t eyey = camera.get_position().y;
	real_t eyez = camera.get_position().z;
	real_t centerx = camera.get_position().x + camera.get_direction().x * camera.focus_dist;
	real_t centery = camera.get_position().y + camera.get_direction().y * camera.focus_dist;
	real_t centerz = camera.get_position().z + camera.get_direction().z * camera.focus_dist;
	real_t upx = camera.get_up().x;
	real_t upy = camera.get_up().y;
	real_t upz = camera.get_up().z;
	
	// set the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, aspect, near_clip, far_clip);

	// set the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyex, eyey, eyez,
			  centerx, centery, centerz,
			  upx, upy, upz);
}

static void set_light_positions(const Scene::LightList & lights)
{
	for(int i=0; i<num_gl_lights && i < (int)lights.size(); ++i)
	{
		const Light & light = lights[i];
		const GLfloat position[] = { (GLfloat)light.position.x,
		                             (GLfloat)light.position.y,
									 (GLfloat)light.position.z, 1 };
		glLightfv(GL_LIGHT0 + i, GL_POSITION, position);
	}
}

static void init_light_properties(const Scene::LightList & lights)
{
	const GLfloat catt = 1;
	const GLfloat latt = 0;
	const GLfloat qatt = 0;
	
	const GLfloat black[] = { 0, 0, 0, 1 };
	const GLfloat white[] = { 1, 1, 1, 1 };

	for(int i=0; i<num_gl_lights; ++i) {
		glDisable(GL_LIGHT0 + i);
	}

	for(int i=0; i<num_gl_lights && i < (int)lights.size(); ++i)
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

/**
 * Render the current scene using opengl to the current frame buffer.
 * @param scene The scene to render.
 * @remark The caller handles double buffering, so do not flip the buffers.
 */
void prj_render(Scene* scene)
{	
	assert(scene);

	set_camera(scene->camera);
	set_light_positions(scene->lights); // light pos are fixed relative to the scene

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	for_each(scene->instances.begin(), scene->instances.end(), &draw);

	treelib_render();
}
