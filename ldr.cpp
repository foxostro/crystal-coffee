/**
 * @file ldr.cpp
 * @brief contains main scene loading function and student-created scenes.
 *
 * @author Eric Butler (edbutler)
 * @author Andrew Fox (arfox)
 */

#include "project.h"
#include "scene.h"
#include "passes.h"
#include "treelib.h"
#include "geom/sphere.h"
#include "geom/trianglesoup.h"
#include "geom/watersurface.h"
#include "geom/pool.h"

#include <iostream>

static RenderMethod * create_tree(Scene * scene)
{
	RenderMethod * rendermethod = new RenderMethod_TreeLib(gen_tree());
	scene->rendermethods.push_back(rendermethod);
	return rendermethod;
}

static RenderMethod * create_tex_sphere(Scene * scene, const char * tex)
{
	Material * mat;
	Texture * diffuse_texture;
	RenderMethod * rendermethod;

	// Earth material
	mat = new Material();
	mat->ambient = Vec3::Ones;
	mat->diffuse = Vec3::Ones;
	mat->shininess = 18;
	mat->specular = Vec3(.1,.1,.1);
	scene->resources.push_back(mat);

	// Earth diffuse texture
	diffuse_texture = new Texture(tex);
	scene->resources.push_back(diffuse_texture);
	
	// Generate sphere geometry
	TriangleSoup sphere = gen_sphere(scene, 4);
	
	// Put it all together to make the "Earth" object
	rendermethod = new RenderMethod_DiffuseTexture(sphere.vertices_buffer,
		                                           sphere.normals_buffer,
		                                           sphere.tcoords_buffer,
												   NULL, // no indices
		                                           mat,
		                                           diffuse_texture);
	scene->rendermethods.push_back(rendermethod);

	return rendermethod;
}

static RenderMethod * create_fresnel_sphere(Scene * scene)
{
	Material * mat;
	Texture * env_map;
	ShaderProgram * fresnel_shader;
	RenderMethod * rendermethod;

	// Base material
	mat = new Material();
	mat->ambient = Vec3(0.2, 0.2, 0.2);
	mat->diffuse = Vec3(0.0, 0.2, 0.3); // blue
	mat->shininess = 16;
	mat->specular = Vec3(0.1, 0.1, 0.1);
	scene->resources.push_back(mat);

	// Create texture resources
	env_map = new Texture("images/spheremap_stpeters.png");
	scene->resources.push_back(env_map);

	// Load and compile the shader
	fresnel_shader = new ShaderProgram("shaders/fresnel_vert.glsl",
	                                   "shaders/fresnel_frag.glsl");
	scene->resources.push_back(fresnel_shader);

	// Generate sphere geometry
	TriangleSoup sphere = gen_sphere(scene, 4);

	// Put it all together
	rendermethod = new RenderMethod_Fresnel(sphere.vertices_buffer,
	                                        sphere.normals_buffer,
										    NULL, // no indices
											fresnel_shader,
											mat,
											env_map,
											1.33);
	scene->rendermethods.push_back(rendermethod);

	return rendermethod;
}

static RenderMethod * create_bumpy_sphere(Scene * scene)
{
	Material * mat;
	Texture * diffuse_map;
	Texture * normal_map;
	Texture * height_map;
	ShaderProgram * parallax_bump_shader;
	RenderMethod * rendermethod;

	// Base material
	mat = new Material();
	mat->ambient = Vec3(0.2, 0.2, 0.2);
	mat->diffuse = Vec3::Ones;
	mat->shininess = 16;
	mat->specular = Vec3(0.1, 0.1, 0.1);
	scene->resources.push_back(mat);

	// Create texture resources
	diffuse_map = new Texture("images/bricks2_diffuse_map.png");
	scene->resources.push_back(diffuse_map);

	normal_map = new Texture("images/bricks2_normal_map.png");
	scene->resources.push_back(normal_map);

	height_map = new Texture("images/bricks2_height_map.png");
	scene->resources.push_back(height_map);

	// Load and compile the shader
	parallax_bump_shader = new ShaderProgram("shaders/bump_vert.glsl",
	                                         "shaders/bump_frag.glsl");
	scene->resources.push_back(parallax_bump_shader);

	// Generate sphere geometry
	TriangleSoup sphere = gen_sphere(scene, 4);

	// Put it all together
	rendermethod = new RenderMethod_BumpMap(sphere.vertices_buffer,
	                                        sphere.normals_buffer,
	                                        sphere.tangents_buffer,
											sphere.tcoords_buffer,
											NULL, // no indices
	                                        parallax_bump_shader,
	                                        mat,
	                                        diffuse_map,
	                                        normal_map,
	                                        height_map);
	scene->rendermethods.push_back(rendermethod);

	return rendermethod;
}

static RenderMethod * create_pool(Scene * scene)
{
	Material * mat;
	Texture * diffuse_map;
	Texture * normal_map;
	Texture * height_map;
	ShaderProgram * parallax_bump_shader;
	RenderMethod * rendermethod;

	assert(scene);

	// Base material
	mat = new Material();
	mat->ambient = Vec3(0.2, 0.2, 0.2);
	mat->diffuse = Vec3::Ones;
	mat->shininess = 16;
	mat->specular = Vec3(0.1, 0.1, 0.1);
	scene->resources.push_back(mat);

	// Create texture resources
	diffuse_map = new Texture("images/bricks2_diffuse_map.png");
	scene->resources.push_back(diffuse_map);

	normal_map = new Texture("images/bricks2_normal_map.png");
	scene->resources.push_back(normal_map);

	height_map = new Texture("images/bricks2_height_map.png");
	scene->resources.push_back(height_map);

	// Load and compile the shader
	parallax_bump_shader = new ShaderProgram("shaders/bump_vert.glsl",
	                                         "shaders/bump_frag.glsl");
	scene->resources.push_back(parallax_bump_shader);

	// Generate sphere geometry
	TriangleSoup pool = gen_pool_geometry(scene);

	// Put it all together
	rendermethod = new RenderMethod_BumpMap(pool.vertices_buffer,
	                                        pool.normals_buffer,
	                                        pool.tangents_buffer,
											pool.tcoords_buffer,
											NULL, // no indices
	                                        parallax_bump_shader,
	                                        mat,
	                                        diffuse_map,
	                                        normal_map,
	                                        height_map);
	scene->rendermethods.push_back(rendermethod);

	return rendermethod;
}

static void ldr_load_example_scene(Scene * scene)
{
	assert(scene);

	// Light the scene
	Light light;
	light.position = Vec3(.4, .7, .8) * 100;
	light.color = Vec3::Ones;
	scene->lights.push_back(light);
	scene->ambient_light = Vec3(.1, .1, .1);

	// The scene has only one pass
	Pass * pass = new StandardPass();
	scene->passes.push_back(pass);

	// Set the camera
	Camera& camera = pass->camera;
	camera.orientation = Quat::Identity;
	camera.position = Vec3(0,0,10);
	camera.focus_dist = 10;
	camera.fov = PI / 3.0;
	camera.near_clip = .1;
	camera.far_clip = 100.0;

	// Set the scene's primary camera
	scene->primary_camera = &(pass->camera);

	// Create an instance of the Earth object
	RenderInstance * earth = new RenderInstance(Mat4(3.0, 0.0, 0.0, 0.0,
	                                                 0.0, 3.0, 0.0, 0.0,
													 0.0, 0.0, 3.0, 0.0,
													 0.0, 0.0, 0.0, 1.0),
	                                            create_tex_sphere(scene, "images/earth.png"));
	pass->instances.push_back(earth);
}

static void ldr_load_fresnel_sphere_scene(Scene * scene)
{
	// Light the scene
	Light light;
	light.position = Vec3(.4, .7, .8) * 100;
	light.color = Vec3::Ones;
	scene->lights.push_back(light);
	scene->ambient_light = Vec3(.1, .1, .1);

	// The scene has only one pass
	Pass * pass = new StandardPass();
	scene->passes.push_back(pass);

	// Create an instance of the object
	RenderInstance * fresnel = new RenderInstance(Mat4(3.0, 0.0, 0.0, 0.0,
	                                                   0.0, 3.0, 0.0, 0.0,
													   0.0, 0.0, 3.0, 0.0,
													   0.0, 0.0, 0.0, 1.0),
												create_fresnel_sphere(scene));
	pass->instances.push_back(fresnel);

	// Set the camera
	Camera& cam = pass->camera;
	cam.orientation = Quat::Identity;
	cam.position = Vec3(0,0,10);
	cam.focus_dist = 10;
	cam.fov = PI / 3.0;
	cam.near_clip = .1;
	cam.far_clip = 100.0;

	// Set the scene's primary camera
	scene->primary_camera = &(pass->camera);
}

static WaterSurface * gen_water_surface(Scene * scene)
{
	WaterSurface * watergeom;
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

	watergeom = new WaterSurface(scene, wave_points, 240, 240);

	scene->tickables.push_back(watergeom);
	
	return watergeom;
}

RenderMethod * create_water(Scene * scene)
{
	Material * mat;
	Texture * spheremap;
	WaterSurface * watergeom;
	ShaderProgram * fresnel;
	RenderMethod * water;



	mat = new Material();
	mat->ambient = Vec3(0.0, 0.2, 0.3);
	mat->diffuse = Vec3(0.0, 0.2, 0.3); // blue water
	mat->shininess = 20;
	mat->specular = Vec3::Ones;
	scene->resources.push_back(mat);



	spheremap = new Texture("images/spheremap_stpeters.png");
	scene->resources.push_back(spheremap);



	fresnel = new ShaderProgram("shaders/fresnel_vert.glsl", "shaders/fresnel_frag.glsl");
	scene->resources.push_back(fresnel);



	watergeom = gen_water_surface(scene);



	water = new RenderMethod_Fresnel(watergeom->vertices_buffer,
	                                 watergeom->normals_buffer,
									 watergeom->indices_buffer,
								     fresnel,
								     mat,
								     spheremap,
								     1.33);
	scene->rendermethods.push_back(water);



	return water;
}

static void ldr_load_pool_scene(Scene * scene)
{
	RenderMethod * pool = create_pool(scene);
	RenderMethod * earth = create_tex_sphere(scene, "images/earth.png");
	RenderMethod * water = create_water(scene);
	RenderMethod * fresnel_sphere = create_fresnel_sphere(scene);
	RenderMethod * swirly_sphere = create_tex_sphere(scene, "images/swirly.png");
	RenderMethod * tree = create_tree(scene);

	// Light the scene
	Light light;
	light.position = Vec3(-4, 8.5, -8) * 30;
	light.color = Vec3(.7,.7,.7);
	scene->lights.push_back(light);
	scene->ambient_light = Vec3(.2,.2,.2);

	// The scene has only one pass
	Pass * pass = new StandardPass();
	scene->passes.push_back(pass);

	pass->instances.push_back(new RenderInstance(Mat4::Identity, pool));

	pass->instances.push_back(new RenderInstance(Mat4(PIX, 0.0, 0.0, 0.0,
	                                                  0.0, 0.4, 0.0, POY - 1.0,
													  0.0, 0.0, PIZ, 0.0,
													  0.0, 0.0, 0.0, 1.0),
												  water));

	const real_t rad = 2.0;

	pass->instances.push_back(new RenderInstance(Mat4(rad, 0.0, 0.0, (POX+PIX)/2,
	                                                  0.0, rad, 0.0, POY+rad,
												      0.0, 0.0, rad, (POZ+PIZ)/2,
												      0.0, 0.0, 0.0, 1.0),
													   earth));

	pass->instances.push_back(new RenderInstance(Mat4(rad, 0.0, 0.0, -(POX+PIX)/2,
	                                                  0.0, rad, 0.0, POY+rad,
													  0.0, 0.0, rad, -(POZ+PIZ)/2,
													  0.0, 0.0, 0.0, 1.0),
												  fresnel_sphere));

	pass->instances.push_back(new RenderInstance(Mat4(1.0, 0.0, 0.0, -(POX+PIX)/2,
	                                                  0.0, 1.0, 0.0, POY,
													  0.0, 0.0, 1.0, (POZ+PIZ)/2,
													  0.0, 0.0, 0.0, 1.0),
												  tree));

	pass->instances.push_back(new RenderInstance(Mat4(rad, 0.0, 0.0, (POX+PIX)/2,
	                                                  0.0, rad, 0.0, POY+rad,
													  0.0, 0.0, rad, -(POZ+PIZ)/2,
													  0.0, 0.0, 0.0, 1.0),
												  swirly_sphere));

	// Set up the camera
	Camera& cam = pass->camera;
	cam.orientation = Quat(-0.0946664, -0.00690199, 0.970616, 0.22112);
	cam.position = Vec3(-2.62381,6.01017,-12.4194);
	cam.focus_dist = 14.0444;
	cam.fov = PI / 3.0;
	cam.near_clip = 1;
	cam.far_clip = 1000.0;

	// Set the scene's primary camera
	scene->primary_camera = &(pass->camera);
}

static void ldr_load_bumpy_sphere_scene(Scene * scene)
{
	// Light the scene
	Light light;
	light.position = Vec3(.4, .7, .8) * 100;
	light.color = Vec3::Ones;
	scene->lights.push_back(light);
	scene->ambient_light = Vec3(.1, .1, .1);

	// The scene has only one pass
	Pass * pass = new StandardPass();
	scene->passes.push_back(pass);

	// Create an instance of the Earth object
	RenderInstance * bumpy = new RenderInstance(Mat4(3.0, 0.0, 0.0, 0.0,
	                                                 0.0, 3.0, 0.0, 0.0,
												     0.0, 0.0, 3.0, 0.0,
												     0.0, 0.0, 0.0, 1.0),
												create_bumpy_sphere(scene));
	pass->instances.push_back(bumpy);

	// Set the camera
	Camera& cam = pass->camera;
	cam.orientation = Quat::Identity;
	cam.position = Vec3(0,0,10);
	cam.focus_dist = 10;
	cam.fov = PI / 3.0;
	cam.near_clip = .1;
	cam.far_clip = 100.0;

	// Set the scene's primary camera
	scene->primary_camera = &(pass->camera);
}

/**
 * Loads the scene with the given num into scene.
 * @param scene The Scene into which to load.
 * @param num The id of the scene to load.
 * @return true on successful load, false otherwise.
 */
bool ldr_load_scene(Scene* scene, int num)
{
    switch(num)
    {
	case 0:
		ldr_load_example_scene(scene);
		break;

	case 1:
		ldr_load_pool_scene(scene);
		break;

	case 2:
		ldr_load_fresnel_sphere_scene(scene);
		break;

	case 3:
		ldr_load_bumpy_sphere_scene(scene);
		break;

    default:
    	std::cout << "#" << num << "does not refer to a scene." << std::endl;
        return false;
    }

    return true;
}

