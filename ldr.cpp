/**
 * @file ldr.cpp
 * @brief contains main scene loading function and student-created scenes.
 *
 * @author Eric Butler (edbutler)
 */

#include "project.h"
#include "scene.h"
#include "geom/sphere.h"
#include "geom/triangle.h"
#include "geom/watersurface.h"

#include <iostream>

static RenderMethod * create_earth(Scene * scene)
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
	diffuse_texture = new Texture("images/earth.png");
	scene->resources.push_back(diffuse_texture);
	
	// Generate sphere geometry
	Sphere sphere = gen_sphere(4);
	
	// Put it all together to make the "Earth" object
	rendermethod = new RenderMethod_DiffuseTexture(sphere.vertices_buffer,
		                                           sphere.normals_buffer,
		                                           sphere.tcoords_buffer,
		                                           mat,
		                                           diffuse_texture);
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
	Sphere sphere = gen_sphere(4);

	// Put it all together
	rendermethod = new RenderMethod_BumpMap(sphere.vertices_buffer,
	                                        sphere.normals_buffer,
											sphere.tangents_buffer,
											sphere.tcoords_buffer,
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
	scene->ambient_light = Vec3(.1, .1, .1);

	// Create an instance of the Earth object
	RenderInstance * earth = new RenderInstance(Mat4(3.0, 0.0, 0.0, 0.0,
	                                                 0.0, 3.0, 0.0, 0.0,
													 0.0, 0.0, 3.0, 0.0,
													 0.0, 0.0, 0.0, 1.0),
	                                            create_earth(scene));
	scene->instances.push_back(earth);

	// Add a light to the scene too
	Light light;
	light.position = Vec3(.4, .7, .8) * 100;
	light.color = Vec3::Ones;
	scene->lights.push_back(light);

	// Set the camera
	Camera& camera = scene->camera;
	camera.orientation = Quat::Identity;
	camera.position = Vec3(0,0,10);
	camera.focus_dist = 10;
	camera.fov = PI / 3.0;
	camera.near_clip = .1;
	camera.far_clip = 100.0;
}

static void ldr_load_scene_1(Scene * scene)
{
	scene->ambient_light = Vec3(.1, .1, .1);

	// Create an instance of the Earth object
	RenderInstance * bumpy = new RenderInstance(Mat4(3.0, 0.0, 0.0, 0.0,
	                                                 0.0, 3.0, 0.0, 0.0,
													 0.0, 0.0, 3.0, 0.0,
													 0.0, 0.0, 0.0, 1.0),
												create_bumpy_sphere(scene));
	scene->instances.push_back(bumpy);

	// Add a light to the scene too
	Light light;
	light.position = Vec3(.4, .7, .8) * 100;
	light.color = Vec3::Ones;
	scene->lights.push_back(light);

	// Set the camera
	Camera& cam = scene->camera;
	cam.orientation = Quat::Identity;
	cam.position = Vec3(0,0,10);
	cam.focus_dist = 10;
	cam.fov = PI / 3.0;
	cam.near_clip = .1;
	cam.far_clip = 100.0;
}

/**
 * Loads the scene with the given num into scene.
 * @param scene The Scene into which to load.
 * @param num The id of the scene to load.
 * @return true on successful load, false otherwise.
 */
bool ldr_load_scene(Scene* scene, int num)
{
    switch (num)
    {
	case 0:
		ldr_load_example_scene(scene);
		break;

	case 1:
		ldr_load_scene_1(scene);
		break;

    default:
    	std::cout << "#" << num << "does not refer to a scene." << std::endl;
        return false;
    }

    return true;
}

