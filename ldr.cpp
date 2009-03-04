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

static void ldr_load_example_scene(Scene* scene)
{
	Material * mat;
	Texture * diffuse_texture;
	RenderMethod * rendermethod;

	scene->ambient_light = Vec3(.1, .1, .1);

	// Earth material
	mat = new Material();
	mat->ambient = Vec3::Ones;
	mat->diffuse = Vec3::Ones;
	mat->phong = Vec3::Ones;
	mat->shininess = 18;
	mat->specular = Vec3(.1,.1,.1);
	scene->resources.push_back(mat);

	// Earth diffuse texture
	diffuse_texture = new Texture("images/earth.png");
	scene->resources.push_back(diffuse_texture);
	
	// Generate sphere geometry
	const int sub = 4;
	Sphere sphere = gen_sphere(sub);
	
	// Transform to apply to the Earth
	Mat4 transform(3.0, 0.0, 0.0, 0.0,
                   0.0, 3.0, 0.0, 0.0,
                   0.0, 0.0, 3.0, 0.0,
	               0.0, 0.0, 0.0, 1.0);
	
	// Put it all together to make the "Earth" object
	rendermethod = new RenderMethod_DiffuseTexture(transform,
		                                           sphere.vertices_buffer,
		                                           sphere.normals_buffer,
		                                           sphere.tcoords_buffer,
		                                           mat,
		                                           diffuse_texture);
	scene->rendermethods.push_back(rendermethod);

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
    case 1:
        ldr_load_example_scene(scene);
        break;
    default:
    	std::cout << "#" << num << "does not refer to a scene." << std::endl;
        return false;
    }

    return true;
}

