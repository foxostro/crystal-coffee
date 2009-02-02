/**
 * @file ldr.cpp
 * @brief contains main scene loading function and student-created scenes.
 *
 * @author Eric Butler (edbutler)
 */

/*
    OPTIONAL: add your own scenes to this file.
 */

#include "project.h"
#include "scene.h"
#include "geom/sphere.h"
#include "geom/triangle.h"
#include "geom/watersurface.h"

static void ldr_load_example_scene(Scene* scene)
{
    Camera& cam = scene->camera;
    cam.orientation = Quat::Identity;
    cam.position = Vec3(0,0,10);
    cam.focus_dist = 10;
    cam.fov = PI / 3.0;
    cam.near_clip = .1;
    cam.far_clip = 100.0;

    scene->ambient_light = Vec3(.1,.1,.1);

    Material* mat;

    mat = new Material();
    mat->ambient = Vec3(0,0,1);
    mat->diffuse = Vec3(0,0,1);
    mat->phong = Vec3(.3,.3,1);
    mat->shininess = 20;
    scene->materials.push_back(mat);

    scene->objects.push_back(
        new Sphere(Vec3::Zero, Quat::Identity, Vec3::Ones, 2, mat));

    Light light;
    light.position = Vec3(.4, .7, .8) * 100;
    light.color = Vec3::Ones;
    scene->lights.push_back(light);
}

/**
 * Loads the scene with the given num into scene.
 * @param scene The Scene into which to load.
 * @param num The id of the scene to load.
 * @return true on successful load, false otherwise.
 */
bool ldr_load_scene(Scene* scene, int num)
{
    // TODO OPTIONAL add function calls to load your own scenes here.
    // scenes [0, MAX_STAFF_SCENE_NUM) are reserved for staff.
    switch (num)
    {
    case 10:
        ldr_load_example_scene(scene);
        break;
    default:
        return false;
    }

    return true;
}

