/**
 * @file scene.cpp
 * @brief Function definitions for the Material, Geometry, UpdatableGeometry,
 *  Camera, Light, and Scene classes.
 *
 * @author Eric Butler (edbutler)
 * @author Kristin Siu (kasiu)
 */

/*
    YOU ARE FREE TO MODIFY THIS FILE, as long as you do not change existing
    constructor signatures or remove existing class members. The staff scene
    loader requires all of those to be intact. You may, however, modify
    anything else, including other function signatures and adding additional
    members.
 */

#include "vec/mat.h"
#include "scene.h"
#include "glheaders.h"
#include "imageio.h"
#include <iostream>

bool app_is_glsl_enabled();

Material::Material():
    diffuse(Vec3::Ones), phong(Vec3::Zero), ambient(Vec3::Ones),
    specular(Vec3::Zero), shininess(0), refraction_index(0),
    texture(0), tex_width(0), tex_height(0), gltex_name(0) {}

Material::~Material()
{
    free(texture);
    
    glDeleteTextures(1, &gltex_name);
}

void Material::load_texture()
{
    // don't load texture if already loaded or filename is blank
    if(!texture && !texture_name.empty())
    {

		std::cout << "loading texture " << texture_name << "...\n";
		texture = imageio_load_image(texture_name.c_str(),
		                             &tex_width, &tex_height);
		                                 
		// Create an OpenGL texture        
		glGenTextures(1, &gltex_name);
		glBindTexture(GL_TEXTURE_2D, gltex_name);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0,
		             GL_RGBA, GL_UNSIGNED_BYTE, texture);
	}
}

Vec3 Material::get_texture_color(const Vec2& tex_coords) const
{
    if (texture) {
        // wrap texture values
        Vec2 tc(tex_coords);
        tc.x = fmod(tc.x, 1);
        tc.y = fmod(tc.y, 1);

        // use nearest sampling to query color
        int tx = static_cast<int>(tc.x * (tex_width - 1));
        int ty = static_cast<int>(tc.y * (tex_height - 1));
        assert(0 <= tx && 0 <= ty && tx < tex_width && ty < tex_height);

        return color_array_to_vector(texture + 4 * (tx + ty * tex_width)).xyz();
    } else
        return Vec3::Ones;
}



Geometry::Geometry():
    position(Vec3::Zero), orientation(Quat::Identity),
    scale(Vec3::Ones), effect(0) {}

Geometry::Geometry(const Vec3& pos, const Quat& ori, const Vec3& scl,
                   Material* mat, Effect* efc):
    position(pos), orientation(ori), scale(scl), material(mat), effect(efc) {}

Geometry::~Geometry() {}

void Geometry::set_material() const
{
	assert(material);

	const GLfloat ambient[] = { material->ambient.x, material->ambient.y, material->ambient.z, 1 };
	const GLfloat diffuse[] = { material->diffuse.x, material->diffuse.y, material->diffuse.z, 1 };
	const GLfloat specular[] = { material->specular.x, material->specular.y, material->specular.z, 1 };
	const GLfloat black[] = { 0, 0, 0, 1 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, black);
	glMaterialf(GL_FRONT, GL_SHININESS, (GLfloat)material->shininess);

	if(effect && app_is_glsl_enabled())
	{
		effect->bind();
	}
	else
	{
#ifdef USE_GLSL
		// Use the fixed function pipeline
		glUseProgramObjectARB(0);
#endif
		
		// Disable texture unit 1
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
		
		if(material->gltex_name)
		{
			// Bind texture unit 0
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, material->gltex_name);
			glEnable(GL_TEXTURE_2D);
		}
		else
		{
			// Disable texture unit 0
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
		}
	}
}

void Geometry::set_transformation() const
{
	glTranslated(position.x, position.y, position.z);

	// orient the object
	Mat4 mat;
	orientation.to_matrix(mat);
	glMultMatrixd(mat.m);

	glScaled(scale.x, scale.y, scale.z);
}

void Geometry::CalculateTriangleTangent(const Vec3 *vertices,
                                        const Vec3 *normals,
                                        const Vec2 *tcoords,
                                        Vec3 *tangents)
{
	/*
	Generate a tangent for each vertex. Do so in a consistent *enough* manner
	that tangents are interpolateable across the surface of the mesh.
	*/
	for(int i=0; i<3; ++i)
	{
		tangents[i] = normals[i].cross(Vec3(0.0, -1.0, 1.0)).normalize();
	}
}

Camera::Camera()
    : position(Vec3::Zero), orientation(Quat::Identity), focus_dist(1),
      fov(PI), aspect(1), near_clip(.1), far_clip(10) {}

const Vec3& Camera::get_position() const
{
    return position;
}

Vec3 Camera::get_direction() const
{
    return orientation * -Vec3::UnitZ;
}

Vec3 Camera::get_up() const
{
    return orientation * Vec3::UnitY;
}

real_t Camera::get_fov_radians() const
{
    return fov;
}

real_t Camera::get_fov_degrees() const
{
    return fov * 180.0 / PI;
}

real_t Camera::get_aspect_ratio() const
{
    return aspect;
}

real_t Camera::get_near_clip() const
{
    return near_clip;
}

real_t Camera::get_far_clip() const
{
    return far_clip;
}

void Camera::translate(const Vec3& v)
{
    position += orientation * v;
}

void Camera::pitch(real_t radians)
{
    rotate(orientation * Vec3::UnitX, radians);
}

void Camera::roll(real_t radians)
{
    rotate(orientation * Vec3::UnitZ, radians);
}

void Camera::yaw(real_t radians)
{
    rotate(orientation * Vec3::UnitY, radians);
}

void Camera::rotate(const Vec3& axis, real_t radians)
{
    orientation = Quat(axis, radians) * orientation;
    orientation.normalize();
}

void Camera::pitch_about_focus(real_t radians)
{
    rotate_about_focus(orientation * Vec3::UnitX, radians);
}

void Camera::yaw_about_focus(real_t radians)
{
    rotate_about_focus(orientation * Vec3::UnitY, radians);
}

void Camera::rotate_about_focus(const Vec3& axis, real_t radians)
{
    // compute rotation, then "swing" camera about focus by that rotation
    Quat rotation(axis, radians);
    Vec3 camdir = orientation * Vec3::UnitZ;
    Vec3 focus = position - (focus_dist * camdir);
    position = focus + ((rotation * camdir) * focus_dist);
    orientation = rotation * orientation;
    orientation.normalize();
}



Light::Light()
    : position(Vec3::Zero), color(Vec3::Ones), intensity(1) {}



Scene::Scene()
    : background(0), ambient_light(Vec3::Zero), refraction_index(1),
      start_time(0), caustic_generator(0) {}

Scene::~Scene()
{
    for (GeometryList::iterator i=objects.begin(); i!=objects.end(); ++i)
        delete *i;
    for (MaterialList::iterator i=materials.begin(); i!=materials.end(); ++i)
        delete *i;
    for (EffectList::iterator i=effects.begin(); i!=effects.end(); ++i)
        delete *i;
    delete background;
}

