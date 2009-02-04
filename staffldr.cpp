/**
 * @file staffldr.cpp
 * @brief Contans the load functions for the staff's built-in test scenes.
 *
 * @author Eric Butler (edbutler)
 */

/*
    DO NOT MODIFY THIS FILE. It contains the staff test scnes and will be
    updated with each project handout. If you want to add your own scenes,
    do so elsewhere. The suggested file for that is ldr.cpp.
 */

#include "project.h"
#include "scene.h"
#include "geom/sphere.h"
#include "geom/triangle.h"
#include "geom/watersurface.h"

static void ldr_load_scene00(Scene* scene)
{
    // "Basic" Scene
    Camera& cam = scene->camera;
    cam.orientation = Quat::Identity;
    cam.position = Vec3(0,0,10);
    cam.focus_dist = 10;
    cam.fov = PI / 3.0;
    cam.near_clip = .1;
    cam.far_clip = 100.0;

    scene->ambient_light = Vec3(.1,.1,.1);
    scene->refraction_index = 1;
    scene->caustic_generator = 0;

    Material* mat;

    mat = new Material();
    mat->ambient = Vec3(.5,0,.5);
    mat->diffuse = Vec3(.5,0,.5);
    mat->phong = Vec3(1,.3,1);
    mat->shininess = 20;
    mat->specular = Vec3(.1,.1,.1);
    mat->refraction_index = 0;
    scene->materials.push_back(mat);

    {
        Vec3 vertices[] = { Vec3(-.5,0,3), Vec3(.5,0,3), Vec3(0,-.5,3) };
        Vec2 tcoords[] = { Vec2::Zero, Vec2::Zero, Vec2::Zero };
        scene->objects.push_back(
            new Triangle(Vec3(0,-1,-3), Quat(Vec3::UnitZ, PI), Vec3(2,4,2),
                         vertices, tcoords, Vec3::UnitZ, mat, 0));
    }

    mat = new Material();
    mat->ambient = Vec3(0,0,1);
    mat->diffuse = Vec3(0,0,1);
    mat->phong = Vec3(.3,.3,1);
    mat->shininess = 20;
    mat->specular = Vec3(.1,.1,.1);
    mat->refraction_index = 0;
    scene->materials.push_back(mat);

    scene->objects.push_back(
        new Sphere(Vec3(-5,3,-2), Quat::Identity, Vec3::Ones, 2, mat));

    mat = new Material();
    mat->ambient = Vec3(0,1,0);
    mat->diffuse = Vec3(.2,1,.2);
    mat->phong = Vec3::Ones;
    mat->shininess = 80;
    mat->specular = Vec3(.8,.8,.8);
    mat->refraction_index = 0;
    scene->materials.push_back(mat);

    scene->objects.push_back(
        new Sphere(Vec3(4,3,-2), Quat::Identity, Vec3::Ones, 1, mat));

    mat = new Material();
    mat->ambient = Vec3(1,0,0);
    mat->diffuse = Vec3(1,.2,.2);
    mat->phong = Vec3::Ones;
    mat->shininess = 40;
    mat->specular = Vec3(.8,.8,.8);
    mat->refraction_index = 0;
    scene->materials.push_back(mat);

    scene->objects.push_back(
        new Sphere(Vec3(5,-1,0), Quat(Vec3::UnitY, PI/2), Vec3(1,1,2), 1, mat));

    Light light;
    light.position = Vec3(.4, .7, .8) * 100;
    light.color = Vec3::Ones;
    scene->lights.push_back(light);
}

// pool boundaries
#define PIX 5
#define POX 9
#define PIZ 5
#define POZ 9
#define POY -.5
#define PIY -4
#define PBY -5

static void create_square(Scene* scene, const Vec3& xaxis, const Vec3& yaxis,
                          const Vec3& corner, const Vec3& normal, Material* mat,
                          const Vec2& tcoord_min, const Vec2& tcoord_unit)
{
    Vec3 maxi = corner + xaxis + yaxis;

    Vec3 vertices[3];
    Vec3 normals[] = {normal, normal, normal};
    Material* materials[] = { mat, mat, mat };
    Vec2 tcoords[3];

    Vec2 dist(xaxis.magnitude(), yaxis.magnitude());

    vertices[0] = maxi;
    vertices[1] = corner;
    vertices[2] = corner + yaxis;
    tcoords[0] = tcoord_min + dist * tcoord_unit;
    tcoords[1] = tcoord_min;
    tcoords[2] = tcoord_min + Vec2(0, dist.y) * tcoord_unit;
    scene->objects.push_back(
        new Triangle(Vec3::Zero, Quat::Identity, Vec3::Ones,
                     vertices, tcoords, normals, materials, 0));
    vertices[0] = corner;
    vertices[1] = maxi;
    vertices[2] = corner + xaxis;
    tcoords[0] = tcoord_min;
    tcoords[1] = tcoord_min + dist * tcoord_unit;
    tcoords[2] = tcoord_min + Vec2(dist.x, 0) * tcoord_unit;
    scene->objects.push_back(
        new Triangle(Vec3::Zero, Quat::Identity, Vec3::Ones,
                     vertices, tcoords, normals, materials, 0));
}

static void create_pool(Scene* scene)
{
    Material* mat = new Material();
    mat->ambient = Vec3(1,0,0);
    mat->diffuse = Vec3(1,0,0);
    mat->phong = Vec3::Zero;
    mat->shininess = 40;
    mat->specular = Vec3(.2,.2,.2);
    mat->refraction_index = 0;
    scene->materials.push_back(mat);

    Vec2 tcmin(0,0);
    Vec2 tcunit(.25,.25);

    // so very, very disgusting. please don't look, your eyes may melt

    // upper corners
    create_square(scene,
                  Vec3(POX-PIX,0,0), Vec3(0,0,2*POZ), Vec3(PIX,POY,-POZ),
                  Vec3::UnitY, mat, tcmin, tcunit);
    create_square(scene,
                  Vec3(POX-PIX,0,0), Vec3(0,0,2*POZ), Vec3(-POX,POY,-POZ),
                  Vec3::UnitY, mat, tcmin, tcunit);
    create_square(scene,
                  Vec3(2*PIX,0,0), Vec3(0,0,POZ-PIZ), Vec3(-PIX,POY,-POZ),
                  Vec3::UnitY, mat, tcmin, tcunit);
    create_square(scene,
                  Vec3(2*PIX,0,0), Vec3(0,0,POZ-PIZ), Vec3(-PIX,POY,PIZ),
                  Vec3::UnitY, mat, tcmin, tcunit);

    // inner sides
    create_square(scene,
                  Vec3(0,0,2*PIZ), Vec3(0,POY-PIY,0), Vec3(-PIX,PIY,-PIZ),
                  Vec3::UnitX, mat, tcmin, tcunit);
    create_square(scene,
                  Vec3(0,0,2*PIZ), Vec3(0,POY-PIY,0), Vec3(PIX,PIY,-PIZ),
                  -Vec3::UnitX, mat, tcmin, tcunit);
    create_square(scene,
                  Vec3(2*PIX,0,0), Vec3(0,POY-PIY,0), Vec3(-PIX,PIY,-PIZ),
                  Vec3::UnitZ, mat, tcmin, tcunit);
    create_square(scene,
                  Vec3(2*PIX,0,0), Vec3(0,POY-PIY,0), Vec3(-PIX,PIY,PIZ),
                  -Vec3::UnitZ, mat, tcmin, tcunit);

    // outer sides
    create_square(scene,
                  Vec3(0,0,2*POZ), Vec3(0,POY-PBY,0), Vec3(-POX,PBY,-POZ),
                  -Vec3::UnitX, mat, tcmin, tcunit);
    create_square(scene,
                  Vec3(0,0,2*POZ), Vec3(0,POY-PBY,0), Vec3(POX,PBY,-POZ),
                  Vec3::UnitX, mat, tcmin, tcunit);
    create_square(scene,
                  Vec3(2*POX,0,0), Vec3(0,POY-PBY,0), Vec3(-POX,PBY,-POZ),
                  -Vec3::UnitZ, mat, tcmin, tcunit);
    create_square(scene,
                  Vec3(2*POX,0,0), Vec3(0,POY-PBY,0), Vec3(-POX,PBY,POZ),
                  Vec3::UnitZ, mat, tcmin, tcunit);

    // bottom
    create_square(scene,
                  Vec3(2*PIX,0,0), Vec3(0,0,2*PIZ), Vec3(-PIX,PIY,-PIZ),
                  Vec3::UnitY, mat, tcmin, tcunit);
    create_square(scene,
                  Vec3(2*POX,0,0), Vec3(0,0,2*POZ), Vec3(-POX,PBY,-POZ),
                  -Vec3::UnitY, mat, tcmin, tcunit);
}

static void ldr_load_scene01(Scene* scene)
{
    // "Pool" Scene

    Camera& cam = scene->camera;
    cam.orientation = Quat(-0.0946664, -0.00690199, 0.970616, 0.22112);
    cam.position = Vec3(-2.62381,6.01017,-12.4194);
    cam.focus_dist = 14.0444;
    cam.fov = PI / 3.0;
    cam.near_clip = 1;
    cam.far_clip = 1000.0;

    scene->ambient_light = Vec3(.1,.1,.1);
    scene->refraction_index = 1;

    create_pool(scene);

    Material* mat;

    mat = new Material();
    mat->ambient = Vec3(0.0, 0.2, 0.3);
    mat->diffuse = Vec3(0.0, 0.2, 0.3); // blue water
    mat->phong = Vec3::Ones;
    mat->shininess = 20;
    mat->specular = Vec3::Ones;
    mat->refraction_index = 1.33;
    scene->materials.push_back(mat);

    WaterSurface::WavePointList wave_points;
    WaterSurface::WavePoint* p;

    wave_points.push_back(WaterSurface::WavePoint());
    p = &wave_points.back();
    p->position = Vec2(.42,.56);
    p->falloff = 2;
    p->coefficient = .3;
    p->timerate = -6*PI;
    p->period = 16*PI;

    wave_points.push_back(WaterSurface::WavePoint());
    p = &wave_points.back();
    p->position = Vec2(-.58,-.30);
    p->falloff = 2;
    p->coefficient = .3;
    p->timerate = -8*PI;
    p->period = 20*PI;

    WaterSurface* water_surface = new WaterSurface(Vec3(0, POY - 1, 0),
                                     Quat::Identity,
                                     Vec3(PIX, 0.4, PIZ),
									 wave_points,
									 128, 128, mat);

    scene->objects.push_back(water_surface);
    scene->updatable_objects.push_back(water_surface);
    scene->caustic_generator = water_surface;

    Light light;
    light.position = Vec3(-4, 8.5, -8) * 30;
    light.color = Vec3(.7,.7,.7);
    scene->lights.push_back(light);
}

bool ldr_load_staff_scene(Scene* scene, int num)
{
    switch (num)
    {
    case 0:
        ldr_load_scene00(scene);
        break;
    case 1:
        ldr_load_scene01(scene);
        break;
    default:
        return false;
        break;
    }

    return true;
}

