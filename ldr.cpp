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
#include "geom/sphere.h"
#include "geom/trianglesoup.h"
#include "geom/watersurface.h"
#include "geom/pool.h"

#include <iostream>

static
boost::shared_ptr<RenderMethod>
create_tex_sphere(Scene * scene,
				  boost::shared_ptr< const Texture > tex,
				  bool include_tcoords = true)
{
	Material mat;
	boost::shared_ptr<RenderMethod> rendermethod;

	assert(scene);

	// material
	mat.ambient = Vec3::Ones;
	mat.diffuse = Vec3::Ones;
	mat.shininess = 18;
	mat.specular = Vec3::Ones * 0.1;
	
	// Generate sphere geometry
	TriangleSoup sphere = gen_sphere(scene, 4);
	
	// Put it all together to make the object
	rendermethod = boost::shared_ptr<RenderMethod>(
		new RenderMethod_DiffuseTexture(sphere.vertices_buffer,
		                                sphere.normals_buffer,
										include_tcoords
										  ? sphere.tcoords_buffer
										  : boost::shared_ptr< BufferObject<Vec2> >(),
										boost::shared_ptr< const BufferObject<index_t> >(), // no indices
		                                mat,
		                                tex));
	scene->rendermethods.push_back(rendermethod);

	return rendermethod;
}

static boost::shared_ptr<RenderMethod> create_tex_sphere(Scene * scene, const char * tex)
{
	boost::shared_ptr<Texture2D> diffuse_texture;

	assert(scene);
	assert(tex);

	diffuse_texture = boost::shared_ptr<Texture2D>(new Texture2D(tex));
	scene->resources.push_back(diffuse_texture);

	return create_tex_sphere(scene, diffuse_texture);
}

static
boost::shared_ptr<RenderMethod>
create_cubemapped_sphere(Scene * scene,
					 const boost::shared_ptr< const CubeMapTexture > &cubemap,
					 const char *vert, const char *frag)
{
	Material mat;
	boost::shared_ptr<RenderMethod> rendermethod;
	boost::shared_ptr<ShaderProgram> shader;

	assert(scene);

	// material
	mat.ambient = Vec3::Ones;
	mat.diffuse = Vec3::Ones;
	mat.shininess = 18;
	mat.specular = Vec3::Ones * 0.1;
	
	// Generate sphere geometry
	TriangleSoup sphere = gen_sphere(scene, 4);

	// Compile a shader for the mirror sphere
	shader = boost::shared_ptr<ShaderProgram>(new ShaderProgram(vert, frag));
	
	// Put it all together to make the object
	rendermethod = boost::shared_ptr<RenderMethod>(new RenderMethod_CubemapReflection(sphere.vertices_buffer,
		                                                                              sphere.normals_buffer,
												                                      boost::shared_ptr< const BufferObject<index_t> >(), // no indices
		                                                                              mat,
		                                                                              cubemap,
																					  shader));
	scene->rendermethods.push_back(rendermethod);

	return rendermethod;
}

static boost::shared_ptr<RenderMethod> create_fresnel_sphere(Scene * scene)
{
	Material mat;
	boost::shared_ptr<Texture2D> spheremap;
	boost::shared_ptr<ShaderProgram> fresnel_shader;
	boost::shared_ptr<RenderMethod> rendermethod;

	// Base material
	mat.ambient = Vec3(0.2, 0.2, 0.2);
	mat.diffuse = Vec3(0.0, 0.2, 0.3); // blue
	mat.shininess = 16;
	mat.specular = Vec3(0.1, 0.1, 0.1);

	// Create texture resources
	spheremap = boost::shared_ptr<Texture2D>(new Texture2D("images/spheremap_stpeters.png"));
	scene->resources.push_back(spheremap);

	// Load and compile the shader
	fresnel_shader = boost::shared_ptr<ShaderProgram>(new ShaderProgram("shaders/fresnel_spheremap_vert.glsl",
	                                   	                                "shaders/fresnel_spheremap_frag.glsl"));
	scene->resources.push_back(fresnel_shader);

	// Generate sphere geometry
	TriangleSoup sphere = gen_sphere(scene, 4);

	// Put it all together
	rendermethod = boost::shared_ptr<RenderMethod>(new RenderMethod_FresnelEnvMap(sphere.vertices_buffer,
	                                                                              sphere.normals_buffer,
																				  boost::shared_ptr< const BufferObject<index_t> >(), // no indices
																				  fresnel_shader,
																				  mat,
																				  spheremap,											         											        
																				  1.33));
	scene->rendermethods.push_back(rendermethod);

	return rendermethod;
}

static boost::shared_ptr<RenderMethod> create_bumpy_sphere(Scene * scene)
{
	Material mat;
	boost::shared_ptr<Texture2D> diffuse_map;
	boost::shared_ptr<Texture2D> normal_map;
	boost::shared_ptr<Texture2D> height_map;
	boost::shared_ptr<ShaderProgram> parallax_bump_shader;
	boost::shared_ptr<RenderMethod> rendermethod;

	// Base material
	mat.ambient = Vec3(0.2, 0.2, 0.2);
	mat.diffuse = Vec3::Ones;
	mat.shininess = 16;
	mat.specular = Vec3(0.1, 0.1, 0.1);

	// Create texture resources
	diffuse_map = boost::shared_ptr<Texture2D>(new Texture2D("images/bricks2_diffuse_map.png"));
	scene->resources.push_back(diffuse_map);

	normal_map = boost::shared_ptr<Texture2D>(new Texture2D("images/bricks2_normal_map.png"));
	scene->resources.push_back(normal_map);

	height_map = boost::shared_ptr<Texture2D>(new Texture2D("images/bricks2_height_map.png"));
	scene->resources.push_back(height_map);

	// Load and compile the shader
	parallax_bump_shader = boost::shared_ptr<ShaderProgram>(new ShaderProgram("shaders/bump_vert.glsl", "shaders/bump_frag.glsl"));
	scene->resources.push_back(parallax_bump_shader);

	// Generate sphere geometry
	TriangleSoup sphere = gen_sphere(scene, 4);

	// Put it all together
	rendermethod = boost::shared_ptr<RenderMethod>(new RenderMethod_BumpMap(sphere.vertices_buffer,
		                                                                    sphere.normals_buffer,
		                                                                    sphere.tangents_buffer,
											                                sphere.tcoords_buffer,
												                            boost::shared_ptr< const BufferObject<index_t> >(), // no indices
		                                                                    parallax_bump_shader,
		                                                                    mat,
		                                                                    diffuse_map,
		                                                                    normal_map,
		                                                                    height_map));
	scene->rendermethods.push_back(rendermethod);

	return rendermethod;
}

static boost::shared_ptr<RenderMethod> create_pool(Scene * scene)
{
	Material mat;
	boost::shared_ptr<Texture2D> diffuse_map;
	boost::shared_ptr<Texture2D> normal_map;
	boost::shared_ptr<Texture2D> height_map;
	boost::shared_ptr<ShaderProgram> parallax_bump_shader;
	boost::shared_ptr<RenderMethod> rendermethod;

	assert(scene);

	// Base material
	mat.ambient = Vec3(0.2, 0.2, 0.2);
	mat.diffuse = Vec3::Ones;
	mat.shininess = 16;
	mat.specular = Vec3(0.1, 0.1, 0.1);

	// Create texture resources
	diffuse_map = boost::shared_ptr<Texture2D>(new Texture2D("images/bricks2_diffuse_map.png"));
	scene->resources.push_back(diffuse_map);

	normal_map = boost::shared_ptr<Texture2D>(new Texture2D("images/bricks2_normal_map.png"));
	scene->resources.push_back(normal_map);

	height_map = boost::shared_ptr<Texture2D>(new Texture2D("images/bricks2_height_map.png"));
	scene->resources.push_back(height_map);

	// Load and compile the shader
	parallax_bump_shader = boost::shared_ptr<ShaderProgram>(new ShaderProgram("shaders/bump_vert.glsl", "shaders/bump_frag.glsl"));
	scene->resources.push_back(parallax_bump_shader);

	// Generate sphere geometry
	TriangleSoup pool = gen_pool_geometry(scene);

	// Put it all together
	rendermethod = boost::shared_ptr<RenderMethod>(new RenderMethod_BumpMap(pool.vertices_buffer,
	                                                                        pool.normals_buffer,
	                                                                        pool.tangents_buffer,
											                                pool.tcoords_buffer,
											                                boost::shared_ptr< const BufferObject<index_t> >(), // no indices
	                                                                        parallax_bump_shader,
	                                                                        mat,
	                                                                        diffuse_map,
	                                                                        normal_map,
	                                                                        height_map));
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
	boost::shared_ptr<StandardPass> pass = boost::shared_ptr<StandardPass>(new StandardPass());
	scene->passes.push_back(pass);

	// Render target for the main framebuffer
	pass->rendertarget = boost::shared_ptr<RenderTarget2D>(); // set to null
	pass->proj = Mat4::perspective(PI / 3.0, 800.0/600.0, 0.1, 100.0);

	// Set the camera
	Camera& camera = pass->camera;
	camera.orientation = Quat::Identity;
	camera.position = Vec3(0,0,10);
	camera.focus_dist = 10;

	// Set the scene's primary camera
	scene->primary_camera = &(pass->camera);

	// Create an instance of the Earth object
	boost::shared_ptr<RenderInstance> earth = boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(3.0, 0.0, 0.0, 0.0,
	                                            	                                                    0.0, 3.0, 0.0, 0.0,
	                                            													    0.0, 0.0, 3.0, 0.0,
	                                            													    0.0, 0.0, 0.0, 1.0),
	                                            	                                               create_tex_sphere(scene,
																								                     "images/earth.png")));
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
	boost::shared_ptr<StandardPass> pass = boost::shared_ptr<StandardPass>(new StandardPass());
	scene->passes.push_back(pass);

	// Render target for the main framebuffer
	pass->rendertarget = boost::shared_ptr<RenderTarget2D>(); // set to null
	pass->proj = Mat4::perspective(PI / 3.0, 800.0/600.0, 0.1, 100.0);

	// Create the cubemap texture from 6 image files.
	boost::shared_ptr<Texture> cubemap = boost::shared_ptr<Texture>(new CubeMapTexture("images/cubemap/cm_left.jpg",
												  	                                   "images/cubemap/cm_right.jpg",
												  								  	   "images/cubemap/cm_top.jpg",
												  									   "images/cubemap/cm_bottom.jpg",
																					   "images/cubemap/cm_back.jpg",
																					   "images/cubemap/cm_front.jpg"));
	scene->resources.push_back(cubemap);

	// Create an instance of the Earth object
	boost::shared_ptr<RenderInstance> earth = boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(3.0, 0.0, 0.0, 4.0,
	                                                                                                    0.0, 3.0, 0.0, 0.0,
													                                                    0.0, 0.0, 3.0, -5.0,
													                                                    0.0, 0.0, 0.0, 1.0),
	                                                                                               create_tex_sphere(scene, "images/earth.png")));
	pass->instances.push_back(earth);

	// Create an instance of a cubemapped sphere
	boost::shared_ptr<RenderInstance> sphere = boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(3.0, 0.0, 0.0, 0.0,
	                                                                                                     0.0, 3.0, 0.0, 0.0,
													                                                     0.0, 0.0, 3.0, 0.0,
													                                                     0.0, 0.0, 0.0, 1.0),
												                                                   create_tex_sphere(scene, cubemap, false)));
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
	boost::shared_ptr<StandardPass> pass = boost::shared_ptr<StandardPass>(new StandardPass());
	scene->passes.push_back(pass);

	// Render target for the main framebuffer
	pass->rendertarget = boost::shared_ptr<RenderTarget2D>(); // set to null
	pass->proj = Mat4::perspective(PI / 3.0, 800.0/600.0, 0.1, 100.0);

	// Create an instance of the object
	const Mat4 obj_space_to_wld_space(3.0, 0.0, 0.0, 0.0,
	                                  0.0, 3.0, 0.0, 0.0,
								      0.0, 0.0, 3.0, 0.0,
								      0.0, 0.0, 0.0, 1.0);
	boost::shared_ptr<RenderInstance> fresnel = boost::shared_ptr<RenderInstance>(new RenderInstance(obj_space_to_wld_space, create_fresnel_sphere(scene)));
	pass->instances.push_back(fresnel);

	// Set the camera
	Camera& cam = pass->camera;
	cam.orientation = Quat::Identity;
	cam.position = Vec3(0,0,10);
	cam.focus_dist = 10;

	// Set the scene's primary camera
	scene->primary_camera = &(pass->camera);
}

static boost::shared_ptr<WaterSurface> gen_water_surface(Scene * scene)
{
	boost::shared_ptr<WaterSurface> watergeom;
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

	watergeom = boost::shared_ptr<WaterSurface>(new WaterSurface(scene, wave_points, 240, 240));

	scene->tickables.push_back(watergeom);
	
	return watergeom;
}

boost::shared_ptr<RenderMethod>
create_water(Scene * scene,
			 const boost::shared_ptr<const Texture> &cubemap)
{
	Material mat;
	boost::shared_ptr<WaterSurface> watergeom;
	boost::shared_ptr<ShaderProgram> shader;
	boost::shared_ptr<RenderMethod> water;

	assert(cubemap);

	mat.ambient = Vec3(0.0, 0.2, 0.3);
	mat.diffuse = Vec3(0.0, 0.2, 0.3); // blue water
	mat.shininess = 20;
	mat.specular = Vec3::Ones;

	shader = boost::shared_ptr<ShaderProgram>(new ShaderProgram("shaders/fresnel_cubemap_vert.glsl",
	                                                            "shaders/fresnel_cubemap_frag.glsl"));
	scene->resources.push_back(shader);



	watergeom = gen_water_surface(scene);


	water = boost::shared_ptr<RenderMethod>(new RenderMethod_FresnelEnvMap(watergeom->vertices_buffer,
	                                                                       watergeom->normals_buffer,
									                                       watergeom->indices_buffer,
								                                           shader,
								                                           mat,
								                                           cubemap,
								                                           1.33));
	scene->rendermethods.push_back(water);

	return water;
}

static void ldr_load_pool_scene(Scene * scene)
{
	const real_t rad = 2.0;

	boost::shared_ptr<Texture2D> spheremap = boost::shared_ptr<Texture2D>(new Texture2D("images/spheremap_stpeters.png"));
	scene->resources.push_back(spheremap);

	boost::shared_ptr<RenderMethod> pool = create_pool(scene);
	boost::shared_ptr<RenderMethod> earth = create_tex_sphere(scene, "images/earth.png");
	boost::shared_ptr<RenderMethod> water = create_water(scene, spheremap);
	boost::shared_ptr<RenderMethod> fresnel_sphere = create_fresnel_sphere(scene);
	boost::shared_ptr<RenderMethod> swirly_sphere = create_tex_sphere(scene, "images/swirly.png");

	// Light the scene
	Light light;
	light.position = Vec3(-4, 8.5, -8) * 30;
	light.color = Vec3(.7,.7,.7);
	scene->lights.push_back(light);
	scene->ambient_light = Vec3(.2,.2,.2);

	// The scene has only one pass
	boost::shared_ptr<StandardPass> pass = boost::shared_ptr<StandardPass>(new StandardPass());
	scene->passes.push_back(pass);

	// Render target for the main framebuffer
	pass->rendertarget = boost::shared_ptr<RenderTarget2D>(); // set to null
	pass->proj = Mat4::perspective(PI / 3.0, 800.0/600.0, 0.1, 100.0);

	pass->instances.push_back(boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4::Identity, pool)));

	pass->instances.push_back(boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(PIX, 0.0, 0.0, 0.0,
	                                                                                    0.0, 0.4, 0.0, POY - 1.0,
	                                                                                    0.0, 0.0, PIZ, 0.0,
	                                                                                    0.0, 0.0, 0.0, 1.0),
	                                                                               water)));

	pass->instances.push_back(boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(rad, 0.0, 0.0, (POX+PIX)/2,
	                                                                                    0.0, rad, 0.0, POY+rad,
												                                        0.0, 0.0, rad, (POZ+PIZ)/2,
												                                        0.0, 0.0, 0.0, 1.0),
													                               earth)));

	pass->instances.push_back(boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(rad, 0.0, 0.0, -(POX+PIX)/2,
	                                                                                    0.0, rad, 0.0, POY+rad,
	                                                                                    0.0, 0.0, rad, -(POZ+PIZ)/2,
	                                                                                    0.0, 0.0, 0.0, 1.0),
	                                                                               fresnel_sphere)));

	pass->instances.push_back(boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(rad, 0.0, 0.0, (POX+PIX)/2,
	                                                                                    0.0, rad, 0.0, POY+rad,
													                                    0.0, 0.0, rad, -(POZ+PIZ)/2,
													                                    0.0, 0.0, 0.0, 1.0),
												                                    swirly_sphere)));

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
	boost::shared_ptr<StandardPass> pass = boost::shared_ptr<StandardPass>(new StandardPass());
	scene->passes.push_back(pass);

	// Render target for the main framebuffer
	pass->rendertarget = boost::shared_ptr<RenderTarget2D>(); // set to null
	pass->proj = Mat4::perspective(PI / 3.0, 800.0/600.0, 0.1, 100.0);

	// Create an instance of the Earth object
	boost::shared_ptr<RenderInstance> bumpy = boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(3.0, 0.0, 0.0, 0.0,
	                                                                                                    0.0, 3.0, 0.0, 0.0,
												                                                        0.0, 0.0, 3.0, 0.0,
												                                                        0.0, 0.0, 0.0, 1.0),
												                                                   create_bumpy_sphere(scene)));
	pass->instances.push_back(bumpy);

	// Set the camera
	Camera& cam = pass->camera;
	cam.orientation = Quat::Identity;
	cam.position = Vec3(0,0,10);
	cam.focus_dist = 10;

	// Set the scene's primary camera
	scene->primary_camera = &(pass->camera);
}

static void ldr_load_rendertarget_scene_1(Scene * scene)
{
	// Light the scene
	Light light;
	light.position = Vec3(.4, .7, .8) * 100;
	light.color = Vec3::Ones;
	scene->lights.push_back(light);
	scene->ambient_light = Vec3(.1, .1, .1);

	/************************************************************************/

	boost::shared_ptr<StandardPass> pass1 = boost::shared_ptr<StandardPass>(new StandardPass());

	// Render target for the main framebuffer
	boost::shared_ptr<RenderTarget2D> rendertarget1 = boost::shared_ptr<RenderTarget2D>(new RenderTarget2D(ivec2(256,256)));
	scene->resources.push_back(rendertarget1);
	pass1->rendertarget = rendertarget1;
	pass1->proj = Mat4::perspective(PI / 3.0, 1.0, 0.1, 100.0);
	pass1->camera.orientation = Quat::Identity;
	pass1->camera.position = Vec3(0,0,10);
	pass1->camera.focus_dist = 10;
	pass1->clear_color = Vec4(0.3, 0.3, 0.3, 1.0);

	// Add a bumpy sphere
	pass1->instances.push_back(boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(3.0, 0.0, 0.0, 0.0,
	                                                                                     0.0, 3.0, 0.0, 0.0,
													                                     0.0, 0.0, 3.0, 0.0,
													                                     0.0, 0.0, 0.0, 1.0),
												                                    create_bumpy_sphere(scene))));

	/************************************************************************/

	boost::shared_ptr<StandardPass> pass2 = boost::shared_ptr<StandardPass>(new StandardPass());

	// Render target for the main framebuffer
	pass2->rendertarget = boost::shared_ptr<RenderTarget2D>(); // set to null
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

		boost::shared_ptr<RenderMethod> r = boost::shared_ptr<RenderMethod>(new RenderMethod_TextureReplace(geom.vertices_buffer,
														                                                    geom.normals_buffer,
														                                                    geom.tcoords_buffer,
																											boost::shared_ptr< const BufferObject<index_t> >(), // no indices
														                                                    rendertarget1));
		scene->rendermethods.push_back(r);
		pass2->instances.push_back(boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4::Identity, r)));
	}

	/************************************************************************/

	// Set the scene's primary camera
	scene->primary_camera = &(pass2->camera);

	// Set up the order of passes
	scene->passes.push_back(pass1);
	scene->passes.push_back(pass2);
}

static TriangleSoup create_square(Scene * scene)
{
	assert(scene);

	std::vector<Face> faces;
	Face a;
	Face b;

	a.normals[0] = a.normals[1] = a.normals[2] = 
	b.normals[0] = b.normals[1] = b.normals[2] = Vec3(-1.0, 0.0, 0.0);

	a.vertices[0] = Vec3(1.0, 1.0, 1.0);
	a.vertices[1] = Vec3(0.0, 1.0, 1.0);
	a.vertices[2] = Vec3(0.0, 0.0, 1.0);

	a.tcoords[0] = Vec2(1.0, 1.0);
	a.tcoords[1] = Vec2(0.0, 1.0);
	a.tcoords[2] = Vec2(0.0, 0.0);

	b.vertices[0] = Vec3(1.0, 1.0, 1.0);
	b.vertices[1] = Vec3(0.0, 0.0, 1.0);
	b.vertices[2] = Vec3(1.0, 0.0, 1.0);

	b.tcoords[0] = Vec2(1.0, 1.0);
	b.tcoords[1] = Vec2(0.0, 0.0);
	b.tcoords[2] = Vec2(1.0, 0.0);

	faces.push_back(a);
	faces.push_back(b);

	return TriangleSoup(scene, faces);
}

static boost::shared_ptr<RenderInstance> create_square(real_t x, real_t y,
							                           real_t w, real_t h,
							                           Scene * scene,
							                           const boost::shared_ptr<Texture2D> tex)
{
	assert(scene);
	assert(tex);

	TriangleSoup geom = create_square(scene);

	boost::shared_ptr<RenderMethod> r = boost::shared_ptr<RenderMethod>(new RenderMethod_TextureReplace(geom.vertices_buffer,
	                                                                                                    geom.normals_buffer,
	                                                                                                    geom.tcoords_buffer,
																										boost::shared_ptr< const BufferObject<index_t> >(), // no indices
	                                                                                                    tex));

	scene->rendermethods.push_back(r);

	return boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(w,   0.0, 0.0, x,
	                                                                 0.0, h,   0.0, y,
								                                     0.0, 0.0, w,   0.0,
								                                     0.0, 0.0, 0.0, 1.0), r));
}

static void ldr_load_rendertarget_scene_2(Scene * scene)
{
	assert(scene);

	enum FACE { FACE_BEGIN=0, LEFT=0, RIGHT, TOP, BOTTOM, FRONT, BACK, FACE_END };
	boost::shared_ptr<StandardPass> pass_main, pass_face[6];
	boost::shared_ptr<RenderTarget2D> rt_face[6];
	Pass::RenderInstanceList instances;

	const real_t rad = 2.0;

	// Light the scene
	Light light;
	light.position = Vec3(.4, .7, .8) * 100;
	light.color = Vec3::Ones;
	scene->lights.push_back(light);
	scene->ambient_light = Vec3(.1, .1, .1);

	/************************************************************************/

	{
		boost::shared_ptr<Texture2D> spheremap = boost::shared_ptr<Texture2D>(new Texture2D("images/spheremap_stpeters.png"));
		scene->resources.push_back(spheremap);

		boost::shared_ptr<RenderMethod> pool = create_pool(scene);
		boost::shared_ptr<RenderMethod> earth = create_tex_sphere(scene, "images/earth.png");
		boost::shared_ptr<RenderMethod> water = create_water(scene, spheremap);
		boost::shared_ptr<RenderMethod> fresnel_sphere = create_fresnel_sphere(scene);
		boost::shared_ptr<RenderMethod> swirly_sphere = create_tex_sphere(scene, "images/swirly.png");

		instances.push_back(boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4::Identity,
		                                                                         pool)));

		instances.push_back(boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(PIX, 0.0, 0.0, 0.0,
		                                                                              0.0, 0.4, 0.0, POY - 1.0,
		                                                                              0.0, 0.0, PIZ, 0.0,
		                                                                              0.0, 0.0, 0.0, 1.0),
		                                                                         water)));

		instances.push_back(boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(rad, 0.0, 0.0, (POX+PIX)/2,
		                                                                              0.0, rad, 0.0, POY+rad,
		                                                                              0.0, 0.0, rad, (POZ+PIZ)/2,
		                                                                              0.0, 0.0, 0.0, 1.0),
		                                                                         earth)));

		instances.push_back(boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(rad, 0.0, 0.0, -(POX+PIX)/2,
		                                                                              0.0, rad, 0.0, POY+rad,
		                                                                              0.0, 0.0, rad, -(POZ+PIZ)/2,
		                                                                              0.0, 0.0, 0.0, 1.0),
		                                                                         fresnel_sphere)));

		instances.push_back(boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(rad, 0.0, 0.0, (POX+PIX)/2,
		                                                                              0.0, rad, 0.0, POY+rad,
		                                                                              0.0, 0.0, rad, -(POZ+PIZ)/2,
		                                                                              0.0, 0.0, 0.0, 1.0),
		                                                                         swirly_sphere)));
	}

	/************************************************************************/

	for(int i=FACE_BEGIN; i<FACE_END; ++i)
	{
		pass_face[i] = boost::shared_ptr<StandardPass>(new StandardPass());
		rt_face[i] = boost::shared_ptr<RenderTarget2D>(new RenderTarget2D(ivec2(128,128)));
		scene->resources.push_back(rt_face[i]);
		pass_face[i]->rendertarget = rt_face[i];
		pass_face[i]->proj = Mat4::perspective(PI / 2.0, 1.0, 0.1, 100.0);
		pass_face[i]->camera.orientation = face_orientation[i];
		pass_face[i]->camera.position = Vec3(0.0, POY+2.0, 0.0);
		pass_face[i]->camera.focus_dist = 1.0;
		pass_face[i]->clear_color = Vec4(0.3, 0.3, 0.3, 1.0);
		pass_face[i]->instances = instances;
	}

	/************************************************************************/

	{
		pass_main = boost::shared_ptr<StandardPass>(new StandardPass());
		pass_main->rendertarget = boost::shared_ptr<RenderTarget2D>(); // set to null; render to the main framebuffer
		pass_main->proj = Mat4::ortho(-2.0, 2.0, -1.0, 2.0, 0.1, 100.0);
		pass_main->camera.orientation = Quat::Identity;
		pass_main->camera.position = Vec3(0,0,20);
		pass_main->camera.focus_dist = 10;

		pass_main->instances.push_back(create_square(-2.0, 0.0, 1.0, 1.0, scene, rt_face[LEFT]));
		pass_main->instances.push_back(create_square(-1.0, 0.0, 1.0, 1.0, scene, rt_face[FRONT]));
		pass_main->instances.push_back(create_square( 0.0, 0.0, 1.0, 1.0, scene, rt_face[RIGHT]));
		pass_main->instances.push_back(create_square(+1.0, 0.0, 1.0, 1.0, scene, rt_face[BACK]));

		pass_main->instances.push_back(create_square(-1.0, +1.0, 1.0, 1.0, scene, rt_face[TOP]));
		pass_main->instances.push_back(create_square(-1.0, -1.0, 1.0, 1.0, scene, rt_face[BOTTOM]));
	}

	/************************************************************************/

	// Set the scene's primary camera
	scene->primary_camera = &(pass_main->camera);

	// Set up the order of passes
	scene->passes.push_back(pass_face[FRONT]);
	scene->passes.push_back(pass_face[BACK]);
	scene->passes.push_back(pass_face[TOP]);
	scene->passes.push_back(pass_face[BOTTOM]);
	scene->passes.push_back(pass_face[LEFT]);
	scene->passes.push_back(pass_face[RIGHT]);
	scene->passes.push_back(pass_main);
}

static
Pass::RenderInstanceList
ldr_load_cubemap_rendertarget_scene_2_setup_instances(Scene * scene,
													  boost::shared_ptr<CubeMapTarget> &cubemap1,
													  boost::shared_ptr<CubeMapTarget> &cubemap2,
													  boost::shared_ptr<CubeMapTarget> &cubemap3,
													  const real_t rad)
{
	Pass::RenderInstanceList instances;

	assert(scene);

	boost::shared_ptr<RenderMethod> pool = create_pool(scene);
	boost::shared_ptr<RenderMethod> earth = create_tex_sphere(scene, "images/earth.png");
	boost::shared_ptr<RenderMethod> water = create_water(scene, cubemap3);
	boost::shared_ptr<RenderMethod> swirly_sphere = create_tex_sphere(scene, "images/swirly.png");
	boost::shared_ptr<RenderMethod> mirror_sphere1 = create_cubemapped_sphere(scene, cubemap1, "shaders/reflect_vert.glsl", "shaders/reflect_frag.glsl");
	boost::shared_ptr<RenderMethod> mirror_sphere2 = create_cubemapped_sphere(scene, cubemap2, "shaders/reflect_vert.glsl", "shaders/reflect_frag.glsl");

	instances.push_back(boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4::Identity,
	                                                                         pool)));

	instances.push_back(boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(PIX, 0.0, 0.0, 0.0,
	                                                                              0.0, 0.4, 0.0, POY - 1.0,
	                                                                              0.0, 0.0, PIZ, 0.0,
	                                                                              0.0, 0.0, 0.0, 1.0),
	                                                                         water)));

	instances.push_back(boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(rad*2, 0.0, 0.0, -(POX+PIX)/2,
		                                                                          0.0, rad*2, 0.0, POY+rad*2,
		                                                                          0.0, 0.0, rad*2, -(POZ+PIZ)/2,
		                                                                          0.0, 0.0, 0.0, 1.0),
		                                                                     earth)));

	instances.push_back(boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(rad, 0.0, 0.0, (POX+PIX)/2,
	                                                                              0.0, rad, 0.0, POY+rad,
	                                                                              0.0, 0.0, rad, (POZ+PIZ)/2,
	                                                                              0.0, 0.0, 0.0, 1.0),
	                                                                         mirror_sphere1)));

	instances.push_back(boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(rad, 0.0, 0.0, (POX+PIX)/2,
	                                                                              0.0, rad, 0.0, POY+rad,
	                                                                              0.0, 0.0, rad, -(POZ+PIZ)/2,
	                                                                              0.0, 0.0, 0.0, 1.0),
	                                                                         mirror_sphere2)));

	return instances;
}

void ldr_load_cubemap_rendertarget_scene_2(Scene * scene)
{
	boost::shared_ptr<CubeMapUpdatePass> pass1, pass2, pass3;
	boost::shared_ptr<StandardPass> pass4;
	boost::shared_ptr<CubeMapTarget> cubemap1, cubemap2, cubemap3;
	const real_t rad = 2.0;

	// Light the scene
	Light light;
	light.position = Vec3(.4, .7, .8) * 100;
	light.color = Vec3::Ones;
	scene->lights.push_back(light);
	scene->ambient_light = Vec3(.1, .1, .1);

	// Create the cubemaps (used for reflections)
	cubemap1 = boost::shared_ptr<CubeMapTarget>(new CubeMapTarget(ivec2(128, 128)));
	cubemap2 = boost::shared_ptr<CubeMapTarget>(new CubeMapTarget(ivec2(128, 128)));
	cubemap3 = boost::shared_ptr<CubeMapTarget>(new CubeMapTarget(ivec2(128, 128)));

	scene->resources.push_back(cubemap1);
	scene->resources.push_back(cubemap2);
	scene->resources.push_back(cubemap3);

	// Cubemap Update Pass 1 (mirrored sphere #1)
	pass1 = boost::shared_ptr<CubeMapUpdatePass>(new CubeMapUpdatePass());
	pass1->cubemaptarget = cubemap1;
	pass1->proj = Mat4::perspective(PI / 2.0, 1.0, 1.0, 50.0);
	pass1->camera.orientation = Quat::Identity; // orientation is irrelevant as given here
	pass1->camera.position = Vec3((POX+PIX)/2, POY+rad, (POZ+PIZ)/2); // position of the first mirrored sphere
	pass1->camera.focus_dist = 1.0;
	pass1->clear_color = Vec4(0.0, 0.0, 0.0, 1.0);

	// Cubemap Update Pass 2 (mirrored sphere #2)
	pass2 = boost::shared_ptr<CubeMapUpdatePass>(new CubeMapUpdatePass());
	pass2->cubemaptarget = cubemap2;
	pass2->proj = Mat4::perspective(PI / 2.0, 1.0, 1.0, 50.0);
	pass2->camera.orientation = Quat::Identity; // orientation is irrelevant as given here
	pass2->camera.position = Vec3((POX+PIX)/2, POY+rad, -(POZ+PIZ)/2); // position of the second mirrored sphere
	pass2->camera.focus_dist = 1.0;
	pass2->clear_color = Vec4(0.0, 0.0, 0.0, 1.0);

	// Cubemap Update Pass 3 (water)
	pass3 = boost::shared_ptr<CubeMapUpdatePass>(new CubeMapUpdatePass());
	pass3->cubemaptarget = cubemap3;
	pass3->proj = Mat4::perspective(PI / 2.0, 1.0, 1.0, 50.0);
	pass3->camera.orientation = Quat::Identity; // orientation is irrelevant as given here
	pass3->camera.position = Vec3(0.0, POY - 1.0, 0.0); // position of the water surface
	pass3->camera.focus_dist = 1.0;
	pass3->clear_color = Vec4(0.3, 0.3, 0.3, 1.0);

	// Framebuffer update pass 
	pass4 = boost::shared_ptr<StandardPass>(new StandardPass());
	pass4->rendertarget = boost::shared_ptr<RenderTarget2D>(); // set to null
	pass4->proj = Mat4::perspective(PI / 3.0, 800.0/600.0, 0.1, 100.0);
	pass4->camera.orientation = Quat::Identity;
	pass4->camera.position = Vec3(0,0,20);
	pass4->camera.focus_dist = 10;

	// Specify geometry to use for each pass
	pass4->instances =
	pass3->instances =
	pass2->instances =
	pass1->instances = ldr_load_cubemap_rendertarget_scene_2_setup_instances(scene, cubemap1, cubemap2, cubemap3, rad);

	// Set the scene's primary camera
	scene->primary_camera = &(pass4->camera);

	// Set up the order of passes
	scene->passes.push_back(pass1);
	scene->passes.push_back(pass2);
	scene->passes.push_back(pass3);
	scene->passes.push_back(pass4);
}

Pass::RenderInstanceList build_pool_geometry(Scene * scene, real_t rad)
{
	assert(scene);

	Pass::RenderInstanceList instances;

	boost::shared_ptr<Texture2D> spheremap = boost::shared_ptr<Texture2D>(new Texture2D("images/spheremap_stpeters.png"));
	scene->resources.push_back(spheremap);

	boost::shared_ptr<RenderMethod> pool = create_pool(scene);
	boost::shared_ptr<RenderMethod> earth = create_tex_sphere(scene, "images/earth.png");
	boost::shared_ptr<RenderMethod> water = create_water(scene, spheremap);
	boost::shared_ptr<RenderMethod> swirly_sphere = create_tex_sphere(scene, "images/swirly.png");

	instances.push_back(boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4::Identity,
	                                                                         pool)));

	instances.push_back(boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(PIX, 0.0, 0.0, 0.0,
	                                                                              0.0, 0.4, 0.0, POY - 1.0,
	                                                                              0.0, 0.0, PIZ, 0.0,
	                                                                              0.0, 0.0, 0.0, 1.0),
	                                                                         water)));

	instances.push_back(boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(rad, 0.0, 0.0, (POX+PIX)/2,
	                                                                              0.0, rad, 0.0, POY+rad,
	                                                                              0.0, 0.0, rad, (POZ+PIZ)/2,
	                                                                              0.0, 0.0, 0.0, 1.0),
	                                                                         swirly_sphere)));

	instances.push_back(boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(rad, 0.0, 0.0, -(POX+PIX)/2,
	                                                                              0.0, rad, 0.0, POY+rad,
	                                                                              0.0, 0.0, rad, -(POZ+PIZ)/2,
	                                                                              0.0, 0.0, 0.0, 1.0),
	                                                                         earth)));

	instances.push_back(boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(rad, 0.0, 0.0, (POX+PIX)/2,
	                                                                              0.0, rad, 0.0, POY+rad,
	                                                                              0.0, 0.0, rad, -(POZ+PIZ)/2,
	                                                                              0.0, 0.0, 0.0, 1.0),
	                                                                         swirly_sphere)));

	return instances;
}

static void ldr_load_cubemap_rendertarget_scene_1(Scene * scene)
{
	assert(scene);

	boost::shared_ptr<StandardPass> pass_main;
	boost::shared_ptr<CubeMapUpdatePass> pass_cubemap;
	boost::shared_ptr<CubeMapTarget> cubemap;
	const real_t rad = 2.0;

	// Light the scene
	Light light;
	light.position = Vec3(.4, .7, .8) * 100;
	light.color = Vec3::Ones;
	scene->lights.push_back(light);
	scene->ambient_light = Vec3(.1, .1, .1);

	/************************************************************************/
	/** Cube Map ************************************************************/
	/************************************************************************/
	{
		pass_cubemap = boost::shared_ptr<CubeMapUpdatePass>(new CubeMapUpdatePass());
		pass_cubemap->cubemaptarget = cubemap = boost::shared_ptr<CubeMapTarget>(new CubeMapTarget(ivec2(128, 128)));
		scene->resources.push_back(cubemap);
		pass_cubemap->proj = Mat4::perspective(PI / 2.0, 1.0, 1.0, 50.0);
		pass_cubemap->camera.orientation = Quat::Identity; // orientation is irrelevant as given here
		pass_cubemap->camera.position = Vec3((POX+PIX)/2, POY+rad, (POZ+PIZ)/2);
		pass_cubemap->camera.focus_dist = 1.0;
		pass_cubemap->instances = build_pool_geometry(scene, rad);
	}

	/************************************************************************/
	/** Main Pass ***********************************************************/
	/************************************************************************/
	{
		pass_main = boost::shared_ptr<StandardPass>(new StandardPass());
		pass_main->rendertarget = boost::shared_ptr<RenderTarget2D>(); // set to null
		pass_main->proj = Mat4::perspective(PI / 3.0, 800.0/600.0, 0.1, 100.0);
		pass_main->camera.orientation = Quat::Identity;
		pass_main->camera.position = Vec3(0,0,10);
		pass_main->camera.focus_dist = 10;
	
		// Create an instance of the Earth object
		boost::shared_ptr<RenderInstance> earth = boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(3.0, 0.0, 0.0, 4.0,
		                                                 0.0, 3.0, 0.0, 0.0,
														 0.0, 0.0, 3.0, -5.0,
														 0.0, 0.0, 0.0, 1.0),
		                                            create_tex_sphere(scene, "images/earth.png")));
		pass_main->instances.push_back(earth);
	
		// Create an instance of a cubemapped sphere
		boost::shared_ptr<RenderInstance> sphere = boost::shared_ptr<RenderInstance>(new RenderInstance(Mat4(3.0, 0.0, 0.0, 0.0,
		                                                  0.0, 3.0, 0.0, 0.0,
														  0.0, 0.0, 3.0, 0.0,
														  0.0, 0.0, 0.0, 1.0),
													create_tex_sphere(scene, cubemap, false)));
		pass_main->instances.push_back(sphere);
	}

	// Set the scene's primary camera
	scene->primary_camera = &(pass_main->camera);

	scene->passes.push_back(pass_cubemap);
	scene->passes.push_back(pass_main);
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
		ldr_load_rendertarget_scene_1(scene);
		break;

	case 2:
		ldr_load_rendertarget_scene_2(scene);
		break;

	case 3:
		ldr_load_cubemap_sphere_scene(scene);
		break;

	case 4:
		ldr_load_cubemap_rendertarget_scene_1(scene);
		break;

	case 5:
		ldr_load_cubemap_rendertarget_scene_2(scene);
		break;

	case 6:
		ldr_load_fresnel_sphere_scene(scene);
		break;

	case 7:
		ldr_load_bumpy_sphere_scene(scene);
		break;

	case 8:
		ldr_load_pool_scene(scene);
		break;

    default:
    	std::cerr << "#" << num << " does not refer to a scene." << std::endl;
        return false;
    }

    return true;
}

