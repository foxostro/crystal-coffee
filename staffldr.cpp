/**
 * @file staffldr.cpp
 * @brief Contans the load functions for the staff's built-in test scenes.
 *
 * @author Eric Butler (edbutler)
 * @author Zeyang Li (zeyangl)
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
	Material* mat;
	Texture *tex;
	RenderMethod *effect;

    // "Basic" Scene
    Camera& cam = scene->camera;
    cam.orientation = Quat::Identity;
    cam.position = Vec3(0,0,10);
    cam.focus_dist = 10;
    cam.fov = PI / 3.0;
    cam.near_clip = .1;
    cam.far_clip = 100.0;

    scene->ambient_light = Vec3(.1,.1,.1);

	// Earth material
    mat = new Material();
    mat->ambient = Vec3::Ones;
    mat->diffuse = Vec3::Ones;
    mat->phong = Vec3::Ones;
    mat->shininess = 18;
    mat->specular = Vec3(.1,.1,.1);
	scene->materials.push_back(mat);

	// Earth diffuse texture
	tex = new Texture("images/earth.png");
	scene->textures.push_back(tex);

	// Earth vertex stream
	Geometry *sphere = new Sphere(Vec3::Zero, Quat::Identity, Vec3(-1,1,1), 3);
	scene->geoms.push_back(sphere);

	// Renders the Earth
	effect = new RenderMethod_DiffuseTexture(sphere, mat, tex);
	scene->render_methods.push_back(effect);

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
                          const Vec3& corner, const Vec3& normal,
                          const Vec2& tcoord_min, const Vec2& tcoord_unit,
						  RenderMethod *effect)
{
	Triangle *triangle;

    Vec3 maxi = corner + xaxis + yaxis;

    Vec3 vertices[3];
    Vec3 normals[] = {normal, normal, normal};
    Vec2 tcoords[3];

#ifdef UV_CLOCKWISE
    Vec2 dist(xaxis.magnitude(), yaxis.magnitude());	
#else
	// computing uvs in counterclockwise order
	Vec2 dist(yaxis.magnitude(), xaxis.magnitude());
#endif

    vertices[0] = maxi;
    vertices[1] = corner;
    vertices[2] = corner + yaxis;
	
    tcoords[0] = tcoord_min + dist * tcoord_unit;
    tcoords[1] = tcoord_min;
#ifdef UV_CLOCKWISE
    tcoords[2] = tcoord_min + Vec2(0, dist.y) * tcoord_unit;
#else
	tcoords[2] = tcoord_min + Vec2(dist.x, 0) * tcoord_unit;
#endif

	triangle = new Triangle(Vec3::Zero,
	                        Quat::Identity,
							Vec3::Ones,
	                        vertices,
							tcoords,
							normals);
    scene->geoms.push_back(triangle);
	effect->add_geom(triangle);

    vertices[0] = corner;
    vertices[1] = maxi;
    vertices[2] = corner + xaxis;
	
    tcoords[0] = tcoord_min;
    tcoords[1] = tcoord_min + dist * tcoord_unit;
#ifdef UV_CLOCKWISE
    tcoords[2] = tcoord_min + Vec2(dist.x, 0) * tcoord_unit;
#else
	tcoords[2] = tcoord_min + Vec2(0, dist.y) * tcoord_unit;
#endif

	triangle = new Triangle(Vec3::Zero,
	                        Quat::Identity,
							Vec3::Ones,
	                        vertices,
							tcoords,
							normals);
	scene->geoms.push_back(triangle);
	effect->add_geom(triangle);
}

static RenderMethod * create_pool_rendermethod(Scene *scene)
{
	Material *mat;
	Texture *diffuse_map, *normal_map, *height_map;
	
	mat = new Material();
    mat->ambient = Vec3(0.7, 0.7, 0.7);
    mat->diffuse = Vec3::Ones;
    mat->phong = Vec3::Ones;
    mat->shininess = 40;
    mat->specular = Vec3(.2,.2,.2);
    scene->materials.push_back(mat);

	diffuse_map = new Texture("images/bricks2_diffuse_map.png");
	scene->textures.push_back(diffuse_map);

	normal_map = new Texture("images/bricks2_normal_map.png");
	scene->textures.push_back(normal_map);

	height_map = new Texture("images/bricks2_height_map.png");
	scene->textures.push_back(height_map);

	// apply bump mapping to the pool
	RenderMethod* bump = new RenderMethod_BumpMap("shaders/bump_vert.glsl",
	                                              "shaders/bump_frag.glsl",
									              NULL,
	                                              mat,
	                                              diffuse_map,
	                                              normal_map,
									              height_map);
	scene->render_methods.push_back(bump);

	return bump;
}

static void create_pool(Scene* scene, RenderMethod *rendermethod)
{
	assert(scene);
	assert(rendermethod);

    Vec2 tcmin(0,0);
    Vec2 tcunit(.25,.25);

    // so very, very disgusting. please don't look, your eyes may melt

    // upper corners
    create_square(scene,
                  Vec3(POX-PIX,0,0), Vec3(0,0,2*POZ), Vec3(PIX,POY,-POZ),
                  Vec3::UnitY, tcmin, tcunit, rendermethod);
    create_square(scene,
                  Vec3(POX-PIX,0,0), Vec3(0,0,2*POZ), Vec3(-POX,POY,-POZ),
                  Vec3::UnitY, tcmin, tcunit, rendermethod);

    create_square(scene,
                  Vec3(2*PIX,0,0), Vec3(0,0,POZ-PIZ), Vec3(-PIX,POY,-POZ),
                  Vec3::UnitY, tcmin, tcunit, rendermethod);
    create_square(scene,
                  Vec3(2*PIX,0,0), Vec3(0,0,POZ-PIZ), Vec3(-PIX,POY,PIZ),
                  Vec3::UnitY, tcmin, tcunit, rendermethod);

    // inner sides

    create_square(scene,
                  Vec3(0,0,2*PIZ), Vec3(0,POY-PIY,0), Vec3(-PIX,PIY,-PIZ),
                  Vec3::UnitX, tcmin, tcunit, rendermethod);

    create_square(scene,
                  -Vec3(2*PIX,0,0), Vec3(0,POY-PIY,0), Vec3(PIX,PIY,-PIZ),
                  Vec3::UnitZ, tcmin, tcunit, rendermethod);

    create_square(scene,
                  -Vec3(0,POY-PIY,0), -Vec3(0,0,2*PIZ), Vec3(PIX,POY,PIZ),
                  -Vec3::UnitX, tcmin, tcunit, rendermethod);

    create_square(scene,
                  Vec3(2*PIX,0,0), Vec3(0,POY-PIY,0), Vec3(-PIX,PIY,PIZ),
                  -Vec3::UnitZ, tcmin, tcunit, rendermethod);

    // outer sides
    create_square(scene,
                  Vec3(0,POY-PBY,0), Vec3(0,0,2*POZ), Vec3(-POX,PBY,-POZ),
                  -Vec3::UnitX, tcmin, tcunit, rendermethod);

    create_square(scene,
                  Vec3(0,0,2*POZ), Vec3(0,POY-PBY,0), Vec3(POX,PBY,-POZ),
                  Vec3::UnitX, tcmin, tcunit, rendermethod);

    create_square(scene,
                  Vec3(2*POX,0,0), Vec3(0,POY-PBY,0), Vec3(-POX,PBY,-POZ),
                  -Vec3::UnitZ, tcmin, tcunit, rendermethod);

    create_square(scene,
                  Vec3(0,POY-PBY,0), Vec3(2*POX,0,0), Vec3(-POX,PBY,POZ),
                  Vec3::UnitZ, tcmin, tcunit, rendermethod);

    // bottom
    create_square(scene,
                  Vec3(0,0,2*PIZ), Vec3(2*PIX,0,0), Vec3(-PIX,PIY,-PIZ),
                  Vec3::UnitY, tcmin, tcunit, rendermethod);
    create_square(scene,
                  Vec3(0,0,2*POZ), Vec3(2*POX,0,0), Vec3(-POX,PBY,-POZ),
                  -Vec3::UnitY, tcmin, tcunit, rendermethod);

}

static void ldr_load_scene01(Scene* scene)
{
	Material* mat;

    // "Pool" Scene

    Camera& cam = scene->camera;
    cam.orientation = Quat(-0.0946664, -0.00690199, 0.970616, 0.22112);
    cam.position = Vec3(-2.62381,6.01017,-12.4194);
    cam.focus_dist = 14.0444;
    cam.fov = PI / 3.0;
    cam.near_clip = 1;
    cam.far_clip = 1000.0;

	// create sphere map
	Texture *spheremap = new Texture("images/spheremap_stpeters.png");
	scene->textures.push_back(spheremap);

    scene->ambient_light = Vec3(.2,.2,.2);

	RenderMethod *bump = create_pool_rendermethod(scene);

    create_pool(scene, bump);

    mat = new Material();
    mat->ambient = Vec3(0.0, 0.2, 0.3);
    mat->diffuse = Vec3(0.0, 0.2, 0.3); // blue water
    mat->phong = Vec3::Ones;
    mat->shininess = 20;
    mat->specular = Vec3::Ones;
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
                                     240, 240);
    scene->geoms.push_back(water_surface);
    scene->updatable_objects.push_back(water_surface);

	real_t refraction_index = 1.33;
	RenderMethod* fresnel = new RenderMethod_Fresnel("shaders/fresnel_vert.glsl",
	                                    "shaders/fresnel_frag.glsl",
										water_surface,
										mat,
										spheremap,
										refraction_index);
	scene->render_methods.push_back(fresnel);

    mat = new Material();
    mat->ambient = Vec3::Ones;
    mat->diffuse = Vec3::Ones;
    mat->phong = Vec3(1,.5,1);
    mat->shininess = 20;
    mat->specular = Vec3(.6,.6,.6);
	scene->materials.push_back(mat);

	Texture *swirly_tex = new Texture("images/swirly.png");
	scene->textures.push_back(swirly_tex);

	RenderMethod *swirly = new RenderMethod_DiffuseTexture(NULL, mat, swirly_tex);
	scene->render_methods.push_back(swirly);

    real_t rad = 2;
	Geometry *sphere;
	
	sphere = new Sphere(Vec3((POX+PIX)/2, POY+rad, (POZ+PIZ)/2), Quat::Identity, Vec3::Ones, rad);
	bump->add_geom(sphere);
    scene->geoms.push_back(sphere);

	sphere = new Sphere(Vec3(-(POX+PIX)/2, POY+rad, -(POZ+PIZ)/2), Quat::Identity, Vec3::Ones, rad);
	swirly->add_geom(sphere);
    scene->geoms.push_back(sphere);

	RenderMethod *plain = new RenderMethod_DiffuseTexture(NULL, mat, swirly_tex);
	scene->render_methods.push_back(plain);

	sphere = new Sphere(Vec3(-(POX+PIX)/2, POY+rad, (POZ+PIZ)/2), Quat::Identity, Vec3::Ones, rad);
	swirly->add_geom(sphere);
    scene->geoms.push_back(sphere);

	sphere = new Sphere(Vec3((POX+PIX)/2, POY+rad, -(POZ+PIZ)/2), Quat::Identity, Vec3::Ones, rad);
	swirly->add_geom(sphere);
    scene->geoms.push_back(sphere);

    Light light;
    light.position = Vec3(-4, 8.5, 8) * 30;
    light.color = Vec3(.7,.7,.7);
    scene->lights.push_back(light);
}

static void ldr_load_scene02(Scene* scene) // Andrew Fox: Bump-mapped Sphere
{
	RenderMethod *bump;
	Geometry *sphere;
	Material *mat;
	Texture *diffuse_map;
	Texture *normal_map;
	Texture *height_map;

    // "Basic" Scene
    Camera& cam = scene->camera;
    cam.orientation = Quat::Identity;
    cam.position = Vec3(0,0,10);
    cam.focus_dist = 10;
    cam.fov = PI / 3.0;
    cam.near_clip = .1;
    cam.far_clip = 100.0;

    scene->ambient_light = Vec3(.1,.1,.1);

    mat = new Material();
    mat->ambient = Vec3(0.2, 0.2, 0.2);
    mat->diffuse = Vec3::Ones;
    mat->phong = Vec3::Ones;
    mat->shininess = 16;
    mat->specular = Vec3(0.2, 0.2, 0.2);
    scene->materials.push_back(mat);

	diffuse_map = new Texture("images/bricks2_diffuse_map.png");
	scene->textures.push_back(diffuse_map);

	normal_map = new Texture("images/bricks2_normal_map.png");
	scene->textures.push_back(normal_map);

	height_map = new Texture("images/bricks2_height_map.png");
	scene->textures.push_back(height_map);

	sphere = new Sphere(Vec3::Zero, Quat::Identity, Vec3(-1,1,1), 3);
	scene->geoms.push_back(sphere);

	bump = new RenderMethod_BumpMap("shaders/bump_vert.glsl",
	                         "shaders/bump_frag.glsl",
							 sphere,
							 mat,
							 diffuse_map,
							 normal_map,
							 height_map);
	scene->render_methods.push_back(bump);

    Light light;
    light.position = Vec3(.4, .7, .8) * 100;
    light.color = Vec3::Ones;
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
    case 2: // bump-mapped sphere
        ldr_load_scene02(scene);
        break;
    default:
        return false;
        break;
    }

    return true;
}

