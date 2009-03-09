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

static RenderMethod * create_tex_sphere(Scene * scene, const Texture * tex, bool include_tcoords = true)
{
	Material * mat;
	RenderMethod * rendermethod;

	assert(scene);
	assert(tex);

	// material
	mat = new Material();
	mat->ambient = Vec3::Ones;
	mat->diffuse = Vec3::Ones;
	mat->shininess = 18;
	mat->specular = Vec3(.1,.1,.1);
	scene->resources.push_back(mat);
	
	// Generate sphere geometry
	TriangleSoup sphere = gen_sphere(scene, 4);
	
	// Put it all together to make the object
	rendermethod = new RenderMethod_DiffuseTexture(sphere.vertices_buffer,
		                                           sphere.normals_buffer,
												   include_tcoords ? sphere.tcoords_buffer : NULL,
												   NULL, // no indices
		                                           mat,
		                                           tex);
	scene->rendermethods.push_back(rendermethod);

	return rendermethod;
}

static RenderMethod * create_tex_sphere(Scene * scene, const char * tex)
{
	Texture * diffuse_texture;

	assert(scene);
	assert(tex);

	diffuse_texture = new Texture(tex);
	scene->resources.push_back(diffuse_texture);

	return create_tex_sphere(scene, diffuse_texture);
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
	fresnel_shader = new ShaderProgram("shaders/fresnel_spheremap_vert.glsl",
	                                   "shaders/fresnel_spheremap_frag.glsl");
	scene->resources.push_back(fresnel_shader);

	// Generate sphere geometry
	TriangleSoup sphere = gen_sphere(scene, 4);

	// Put it all together
	rendermethod = new RenderMethod_FresnelSphereMap(sphere.vertices_buffer,
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

	// Render target for the main framebuffer
	RenderTarget * rendertarget = new NullRenderTarget();
	scene->resources.push_back(rendertarget);
	pass->rendertarget = rendertarget;
	pass->proj = Mat4::perspective(PI / 3.0, 800.0/600.0, 0.1, 100.0);

	// Set the camera
	Camera& camera = pass->camera;
	camera.orientation = Quat::Identity;
	camera.position = Vec3(0,0,10);
	camera.focus_dist = 10;

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

static void ldr_load_cubemap_sphere_scene(Scene * scene)
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

	// Render target for the main framebuffer
	RenderTarget * rendertarget = new NullRenderTarget();
	scene->resources.push_back(rendertarget);
	pass->rendertarget = rendertarget;
	pass->proj = Mat4::perspective(PI / 3.0, 800.0/600.0, 0.1, 100.0);

	// Create the cubemap texture from 6 image files.
	CubeMapTexture * cubemap = new CubeMapTexture("images/cubemap/cm_left.jpg",
	                                              "images/cubemap/cm_right.jpg",
												  "images/cubemap/cm_top.jpg",
												  "images/cubemap/cm_bottom.jpg",
												  "images/cubemap/cm_back.jpg",
												  "images/cubemap/cm_front.jpg");
	scene->resources.push_back(cubemap);

	// Create an instance of the Earth object
	RenderInstance * earth = new RenderInstance(Mat4(3.0, 0.0, 0.0, 4.0,
	                                                 0.0, 3.0, 0.0, 0.0,
													 0.0, 0.0, 3.0, -5.0,
													 0.0, 0.0, 0.0, 1.0),
	                                            create_tex_sphere(scene, "images/earth.png"));
	pass->instances.push_back(earth);

	// Create an instance of a cubemapped sphere
	RenderInstance * sphere = new RenderInstance(Mat4(3.0, 0.0, 0.0, 0.0,
	                                                  0.0, 3.0, 0.0, 0.0,
													  0.0, 0.0, 3.0, 0.0,
													  0.0, 0.0, 0.0, 1.0),
												create_tex_sphere(scene, cubemap, false));
	pass->instances.push_back(sphere);

	// Set the camera
	Camera& cam = pass->camera;
	cam.orientation = Quat::Identity;
	cam.position = Vec3(0,0,10);
	cam.focus_dist = 10;

	// Set the scene's primary camera
	scene->primary_camera = &(pass->camera);
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

	// Render target for the main framebuffer
	RenderTarget * rendertarget = new NullRenderTarget();
	scene->resources.push_back(rendertarget);
	pass->rendertarget = rendertarget;
	pass->proj = Mat4::perspective(PI / 3.0, 800.0/600.0, 0.1, 100.0);

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

RenderMethod * create_water(Scene * scene, const Texture * tex)
{
	Material * mat;
	WaterSurface * watergeom;
	ShaderProgram * shader;
	RenderMethod * water;

	assert(tex);

	mat = new Material();
	mat->ambient = Vec3(0.0, 0.2, 0.3);
	mat->diffuse = Vec3(0.0, 0.2, 0.3); // blue water
	mat->shininess = 20;
	mat->specular = Vec3::Ones;
	scene->resources.push_back(mat);


	shader = new ShaderProgram("shaders/fresnel_spheremap_vert.glsl",
	                           "shaders/fresnel_spheremap_frag.glsl");
	scene->resources.push_back(shader);



	watergeom = gen_water_surface(scene);


	water = new RenderMethod_FresnelSphereMap(watergeom->vertices_buffer,
	                                          watergeom->normals_buffer,
									          watergeom->indices_buffer,
								              shader,
								              mat,
								              tex,
								              1.33);
	scene->rendermethods.push_back(water);



	return water;
}

static void ldr_load_pool_scene(Scene * scene)
{
	Texture * spheremap = new Texture("images/spheremap_stpeters.png");
	scene->resources.push_back(spheremap);

	RenderMethod * pool = create_pool(scene);
	RenderMethod * earth = create_tex_sphere(scene, "images/earth.png");
	RenderMethod * water = create_water(scene, spheremap);
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

	// Render target for the main framebuffer
	RenderTarget * rendertarget = new NullRenderTarget();
	scene->resources.push_back(rendertarget);
	pass->rendertarget = rendertarget;
	pass->proj = Mat4::perspective(PI / 3.0, 800.0/600.0, 0.1, 100.0);

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

	// Render target for the main framebuffer
	RenderTarget * rendertarget = new NullRenderTarget();
	scene->resources.push_back(rendertarget);
	pass->rendertarget = rendertarget;
	pass->proj = Mat4::perspective(PI / 3.0, 800.0/600.0, 0.1, 100.0);

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

	// Set the scene's primary camera
	scene->primary_camera = &(pass->camera);
}

static void ldr_load_rendertarget_scene(Scene * scene)
{
	// Light the scene
	Light light;
	light.position = Vec3(.4, .7, .8) * 100;
	light.color = Vec3::Ones;
	scene->lights.push_back(light);
	scene->ambient_light = Vec3(.1, .1, .1);

	/************************************************************************/

	Pass * pass1 = new StandardPass();

	// Render target for the main framebuffer
	RenderTarget * rendertarget1 = new RenderTarget(ivec2(256,256));
	scene->resources.push_back(rendertarget1);
	pass1->rendertarget = rendertarget1;
	pass1->proj = Mat4::perspective(PI / 3.0, 1.0, 0.1, 100.0);
	pass1->camera.orientation = Quat::Identity;
	pass1->camera.position = Vec3(0,0,10);
	pass1->camera.focus_dist = 10;
	pass1->clear_color = Vec4(0.3, 0.3, 0.3, 1.0);

	// Add a bumpy sphere
	pass1->instances.push_back(new RenderInstance(Mat4(3.0, 0.0, 0.0, 0.0,
	                                                   0.0, 3.0, 0.0, 0.0,
													   0.0, 0.0, 3.0, 0.0,
													   0.0, 0.0, 0.0, 1.0),
												  create_bumpy_sphere(scene)));

	/************************************************************************/

	Pass * pass2 = new StandardPass();

	// Render target for the main framebuffer
	RenderTarget * rendertarget2 = new NullRenderTarget();
	scene->resources.push_back(rendertarget2);
	pass2->rendertarget = rendertarget2;
	pass2->proj = Mat4::perspective(PI / 3.0, 800.0/600.0, 0.1, 100.0);
	pass2->camera.orientation = Quat::Identity;
	pass2->camera.position = Vec3(0,0,20);
	pass2->camera.focus_dist = 10;

	{
		std::vector<Face> faces;
		Face a;
		Face b;

		a.normals[0] = a.normals[1] = a.normals[2] = 
		b.normals[0] = b.normals[1] = b.normals[2] = Vec3(-1.0, 0.0, 0.0);

		a.vertices[0] = Vec3(5.0, 5.0, 5.0);
		a.vertices[1] = Vec3(0.0, 5.0, 5.0);
		a.vertices[2] = Vec3(0.0, 0.0, 5.0);

		a.tcoords[0] = Vec2(1.0, 1.0);
		a.tcoords[1] = Vec2(0.0, 1.0);
		a.tcoords[2] = Vec2(0.0, 0.0);

		b.vertices[0] = Vec3(5.0, 5.0, 5.0);
		b.vertices[1] = Vec3(0.0, 0.0, 5.0);
		b.vertices[2] = Vec3(5.0, 0.0, 5.0);

		b.tcoords[0] = Vec2(1.0, 1.0);
		b.tcoords[1] = Vec2(0.0, 0.0);
		b.tcoords[2] = Vec2(1.0, 0.0);

		faces.push_back(a);
		faces.push_back(b);

		TriangleSoup geom(scene, faces);

		RenderMethod * r = new RenderMethod_TextureReplace(geom.vertices_buffer,
														   geom.normals_buffer,
														   geom.tcoords_buffer,
														   NULL,
														   rendertarget1);
		scene->rendermethods.push_back(r);
		pass2->instances.push_back(new RenderInstance(Mat4::Identity, r));
	}

	/************************************************************************/

	// Set the scene's primary camera
	scene->primary_camera = &(pass2->camera);

	// Set up the order of passes
	scene->passes.push_back(pass1);
	scene->passes.push_back(pass2);
}

static void ldr_load_rendertarget_scene_2(Scene * scene)
{
	Pass * pass1, * pass2;
	RenderTarget * rendertarget1;
	real_t aspect = 800.0 / 600.0;

	// Light the scene
	Light light;
	light.position = Vec3(.4, .7, .8) * 100;
	light.color = Vec3::Ones;
	scene->lights.push_back(light);
	scene->ambient_light = Vec3(.1, .1, .1);

	/************************************************************************/

	{
		pass1 = new StandardPass();

		// Render target for the main framebuffer
		rendertarget1 = new RenderTarget(ivec2(800,600));
		scene->resources.push_back(rendertarget1);
		pass1->rendertarget = rendertarget1;
		pass1->proj = Mat4::perspective(PI / 3.0, aspect, 0.1, 100.0);
		pass1->camera.orientation = Quat(-0.0946664, -0.00690199, 0.970616, 0.22112);
		pass1->camera.position = Vec3(-2.62381,6.01017,-12.4194);
		pass1->camera.focus_dist = 14.0444;
		pass1->clear_color = Vec4(0.3, 0.3, 0.3, 1.0);

		Texture * spheremap = new Texture("images/spheremap_stpeters.png");
		scene->resources.push_back(spheremap);

		RenderMethod * pool = create_pool(scene);
		RenderMethod * earth = create_tex_sphere(scene, "images/earth.png");
		RenderMethod * water = create_water(scene, spheremap);
		RenderMethod * fresnel_sphere = create_fresnel_sphere(scene);
		RenderMethod * swirly_sphere = create_tex_sphere(scene, "images/swirly.png");
		RenderMethod * tree = create_tree(scene);

		pass1->instances.push_back(new RenderInstance(Mat4::Identity, pool));

		pass1->instances.push_back(new RenderInstance(Mat4(PIX, 0.0, 0.0, 0.0,
														  0.0, 0.4, 0.0, POY - 1.0,
														  0.0, 0.0, PIZ, 0.0,
														  0.0, 0.0, 0.0, 1.0),
													  water));

		const real_t rad = 2.0;

		pass1->instances.push_back(new RenderInstance(Mat4(rad, 0.0, 0.0, (POX+PIX)/2,
														  0.0, rad, 0.0, POY+rad,
														  0.0, 0.0, rad, (POZ+PIZ)/2,
														  0.0, 0.0, 0.0, 1.0),
														   earth));

		pass1->instances.push_back(new RenderInstance(Mat4(rad, 0.0, 0.0, -(POX+PIX)/2,
														  0.0, rad, 0.0, POY+rad,
														  0.0, 0.0, rad, -(POZ+PIZ)/2,
														  0.0, 0.0, 0.0, 1.0),
													  fresnel_sphere));

		pass1->instances.push_back(new RenderInstance(Mat4(1.0, 0.0, 0.0, -(POX+PIX)/2,
														  0.0, 1.0, 0.0, POY,
														  0.0, 0.0, 1.0, (POZ+PIZ)/2,
														  0.0, 0.0, 0.0, 1.0),
													  tree));

		pass1->instances.push_back(new RenderInstance(Mat4(rad, 0.0, 0.0, (POX+PIX)/2,
														  0.0, rad, 0.0, POY+rad,
														  0.0, 0.0, rad, -(POZ+PIZ)/2,
														  0.0, 0.0, 0.0, 1.0),
													  swirly_sphere));
	}

	/************************************************************************/

	{
		pass2 = new StandardPass();

		// Render target for the main framebuffer
		RenderTarget * rendertarget2 = new NullRenderTarget();
		scene->resources.push_back(rendertarget2);
		pass2->rendertarget = rendertarget2;
		pass2->proj = Mat4::perspective(PI / 3.0, 800.0/600.0, 0.1, 100.0);
		pass2->camera.orientation = Quat::Identity;
		pass2->camera.position = Vec3(0,0,20);
		pass2->camera.focus_dist = 10;

		std::vector<Face> faces;
		Face a;
		Face b;

		a.normals[0] = a.normals[1] = a.normals[2] = 
		b.normals[0] = b.normals[1] = b.normals[2] = Vec3(-1.0, 0.0, 0.0);

		a.vertices[0] = Vec3(5.0 * aspect, 5.0, 5.0);
		a.vertices[1] = Vec3(0.0, 5.0, 5.0);
		a.vertices[2] = Vec3(0.0, 0.0, 5.0);

		a.tcoords[0] = Vec2(1.0, 1.0);
		a.tcoords[1] = Vec2(0.0, 1.0);
		a.tcoords[2] = Vec2(0.0, 0.0);

		b.vertices[0] = Vec3(5.0 * aspect, 5.0, 5.0);
		b.vertices[1] = Vec3(0.0, 0.0, 5.0);
		b.vertices[2] = Vec3(5.0 * aspect, 0.0, 5.0);

		b.tcoords[0] = Vec2(1.0, 1.0);
		b.tcoords[1] = Vec2(0.0, 0.0);
		b.tcoords[2] = Vec2(1.0, 0.0);

		faces.push_back(a);
		faces.push_back(b);

		TriangleSoup geom(scene, faces);

		RenderMethod * r = new RenderMethod_TextureReplace(geom.vertices_buffer,
														   geom.normals_buffer,
														   geom.tcoords_buffer,
														   NULL,
														   rendertarget1);
		scene->rendermethods.push_back(r);
		pass2->instances.push_back(new RenderInstance(Mat4::Identity, r));
	}

	/************************************************************************/

	// Set the scene's primary camera
	scene->primary_camera = &(pass2->camera);

	// Set up the order of passes
	scene->passes.push_back(pass1);
	scene->passes.push_back(pass2);
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
		ldr_load_cubemap_sphere_scene(scene);
		break;

	case 2:
		ldr_load_rendertarget_scene_2(scene);
		break;

	case 3:
		ldr_load_fresnel_sphere_scene(scene);
		break;

	case 4:
		ldr_load_bumpy_sphere_scene(scene);
		break;

	case 5:
		ldr_load_pool_scene(scene);
		break;

	case 6:
		ldr_load_rendertarget_scene(scene);
		break;

    default:
    	std::cout << "#" << num << " does not refer to a scene." << std::endl;
        return false;
    }

    return true;
}

