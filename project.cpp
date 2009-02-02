/**
 * @file project.cpp
 * @brief The rendering/update code.
 *
 * @author Your Name (andrewid)
 * @bug Unimplemented.
 */

/*
    EDIT THIS FILE FOR P1.
 */

#include "glheaders.h"
#include "project.h"
#include "scene.h"
#include "geom/sphere.h"
#include <iostream>

#define PERIOD 0.01

// current absolute simulation time for the current scene
static real_t sim_time;

/**
 * Initializes all state for the given scene.
 * @param scene The next scene that will be drawn.
 * @param is_gl_context True if there is a valid opengl context. If false, then
 *  no opengl calls should be made. Only state for raytracing should be set up.
 * @remark Unless you plan on using the --raytrace command line option, you may
 *  ignore is_gl_context. It will always be true when the program is run without
 *  that argument.
 */
void prj_initialize(Scene* scene, bool is_gl_context)
{
    // reset scene time
    sim_time = scene->start_time;

    // TODO P1 perform scene intialization

    if (is_gl_context) {
        // TODO P1 perform scene opengl initialization
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }
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
    // increment time and update all updatable geometries
    sim_time += PERIOD;
    Scene::UpdatableGeometryList& list = scene->updatable_objects;
    for (Scene::UpdatableGeometryList::iterator i = list.begin();
            i != list.end(); ++i)
        (*i)->update(sim_time);
}

/**
 * Render the current scene using opengl to the current frame buffer.
 * @param scene The scene to render.
 * @remark The caller handles double buffering, so do not flip the buffers.
 */
void prj_render(Scene* scene)
{
    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO P1 render scene using opengl

    // test render code
    glBegin(GL_TRIANGLES);
    glColor3d(1.0, 0.0, 0.0);
    glVertex3d(-.25, -.25, 0);
    glColor3d(0.0, 1.0, 0.0);
    glVertex3d(.25, -.25, 0);
    glColor3d(0.0, 0.0, 1.0);
    glVertex3d(0, .35, 0);
    glEnd();
}

