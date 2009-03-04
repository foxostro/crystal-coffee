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

template<typename ELEMENT>
BufferObject<ELEMENT>::~BufferObject() {
	glDeleteBuffers(1, &handle);
	delete [] buffer;
}

template<typename ELEMENT>
BufferObject<ELEMENT>::BufferObject()
: locked(false),
  numElements(0),
  buffer(0),
  handle(0),
  usage(STREAM_DRAW) {
	// Do Nothing
}

template<typename ELEMENT>
BufferObject<ELEMENT>::BufferObject(int numElements,
                                        const ELEMENT *buffer)
: locked(false),
  numElements(0),
  buffer(0),
  handle(0),
  usage(STREAM_DRAW) {
	recreate(numElements, buffer, STREAM_DRAW);
}

template<typename ELEMENT>
BufferObject<ELEMENT>::BufferObject(const BufferObject &copyMe)
: locked(false),
  numElements(0),
  buffer(0),
  handle(0),
  usage(STREAM_DRAW) {
	recreate(copyMe.numElements, copyMe.buffer, copyMe.usage);
}

template<typename ELEMENT>
BufferObject<ELEMENT> * BufferObject<ELEMENT>::clone(void) const {
	return new BufferObject<ELEMENT>(*this);
}

template<typename ELEMENT>
void BufferObject<ELEMENT>::recreate(int numElements,
                                     const ELEMENT *buffer,
                                     BUFFER_USAGE usage) {
	assert(!locked && "Cannot realloc a locked buffer!");
	assert(numElements>=0 && "Parameter \'numElements\' < 0");
	
	this->usage = usage;
	
	create_cpu_buffer(numElements, buffer);
	create_gpu_buffer(numElements, buffer, getGLUsageToken(usage));
}

template<typename ELEMENT>
int BufferObject<ELEMENT>::getNumber() const {
	return numElements;
}

template<typename ELEMENT>
void BufferObject<ELEMENT>::bind() const {
	assert(!locked && "Cannot bind buffer for use when the buffer is locked!");
	glBindBuffer(getTarget(), handle);
}

template<typename ELEMENT>
ELEMENT * BufferObject<ELEMENT>::lock() {
	GLvoid * mapped_buffer = NULL;
	
	assert(!locked && "Cannot lock a buffer that is already locked!");
	locked=true;
	
	glBindBuffer(getTarget(), handle);
	mapped_buffer = glMapBuffer(getTarget(), GL_READ_WRITE);
	
	return (ELEMENT*)mapped_buffer;
}

template<typename ELEMENT>
ELEMENT * BufferObject<ELEMENT>::read_lock() {
	GLvoid * mapped_buffer = NULL;
	
	assert(!locked && "Cannot lock a buffer that is already locked!");
	locked=true;
	
	glBindBuffer(getTarget(), handle);
	mapped_buffer = glMapBuffer(getTarget(), GL_READ_ONLY);
	
	return (ELEMENT*)mapped_buffer;
}

template<typename ELEMENT>
void BufferObject<ELEMENT>::unlock() const {
	assert(locked && "Cannot unlock a buffer that is not locked!");
	locked=false;
	
	glBindBuffer(getTarget(), handle);
	glUnmapBuffer(getTarget());
}

template<typename ELEMENT>
void BufferObject<ELEMENT>::create_cpu_buffer( int numElements, const ELEMENT * buffer ) {
	delete [] (this->buffer);
	this->buffer = 0;
	this->numElements = numElements;
	
	if (numElements>0) {
		this->buffer = new ELEMENT[numElements];
		this->numElements = numElements;
		
		if (buffer != 0) {
			memcpy(this->buffer, buffer, sizeof(ELEMENT)*numElements);
		} else {
			memset(this->buffer, 0, sizeof(ELEMENT)*numElements);
		}
	}
}

template<typename ELEMENT>
void BufferObject<ELEMENT>::create_gpu_buffer(int numElements,
  const ELEMENT * buffer,
  GLenum usage) {
	GLenum target = getTarget();
		
	// Create and fill a buffer object on the GPU
	glGenBuffers(1, &handle);
	glBindBuffer(target, handle);
	glBufferData(target,
	             sizeof(ELEMENT) * numElements,
	             buffer,
	             usage);
}

template<typename ELEMENT>
GLenum BufferObject<ELEMENT>::getTarget() {
	return GL_ARRAY_BUFFER;
}

/**
Element array buffers have a different target
specification from vertex array buffers.
*/
template<> GLenum BufferObject<index_t>::getTarget() {
	return GL_ELEMENT_ARRAY_BUFFER;
}

template<typename ELEMENT>
GLenum BufferObject<ELEMENT>::getGLUsageToken(BUFFER_USAGE usage) {
	switch (usage) {
	case STREAM_DRAW:
		return GL_STREAM_DRAW;
	case STREAM_READ:
		return GL_STREAM_READ;
	case STREAM_COPY:
		return GL_STREAM_COPY;
	case STATIC_DRAW:
		return GL_STATIC_DRAW;
	case STATIC_READ:
		return GL_STATIC_READ;
	case STATIC_COPY:
		return GL_STATIC_COPY;
	case DYNAMIC_DRAW:
		return GL_DYNAMIC_DRAW;
	case DYNAMIC_READ:
		return GL_DYNAMIC_READ;
	case DYNAMIC_COPY:
		return GL_DYNAMIC_COPY;
	default:
		assert(!"Invalid enumerant");
		return 0;
	}
}

// template class instantiations
// (see http://www.codeproject.com/cpp/templatesourceorg.asp)
template class BufferObject<Vec4>;
template class BufferObject<Vec3>;
template class BufferObject<Vec2>;
template class BufferObject<index_t>;

Material::Material()
: diffuse(Vec3::Ones),
  phong(Vec3::Zero),
  ambient(Vec3::Ones),
  specular(Vec3::Zero),
  shininess(0)
{
	// Do nothing
}

Material::~Material()
{
	// Do nothing
}

void Material::bind() const
{
	const GLfloat c_a[] = { ambient.x, ambient.y, ambient.z, 1 };
	const GLfloat c_d[] = { diffuse.x, diffuse.y, diffuse.z, 1 };
	const GLfloat c_s[] = { specular.x, specular.y, specular.z, 1 };
	const GLfloat black[] = { 0, 0, 0, 1 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, c_a);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, c_d);
	glMaterialfv(GL_FRONT, GL_SPECULAR, c_s);
	glMaterialfv(GL_FRONT, GL_EMISSION, black);
	glMaterialf(GL_FRONT, GL_SHININESS, (GLfloat)shininess);
}

Texture::Texture(const std::string &tex_name)
: texture_name(tex_name),
  gltex_name(0)
{
	// Do Nothing
}

Texture::~Texture()
{
	glDeleteTextures(1, &gltex_name);
}

void Texture::load_texture()
{
	unsigned char* texture;
	int tex_width, tex_height;

	// don't load texture if already loaded or filename is blank
	if(gltex_name || texture_name.empty())
		return;

	std::cout << "loading texture " << texture_name << "...\n";
	texture = imageio_load_image(texture_name.c_str(),
	                             &tex_width,
	                             &tex_height);

	// Create an OpenGL texture        
	glGenTextures(1, &gltex_name);
	glBindTexture(GL_TEXTURE_2D, gltex_name);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0,
	             GL_RGBA, GL_UNSIGNED_BYTE, texture);

	free(texture);
}

void calculate_triangle_tangent(const Vec3 *vertices,
                                const Vec3 *normals,
                                const Vec2 *tcoords,
                                Vec4 *tangents)
{
	// Source: <http://www.terathon.com/code/tangent.html>
	
    const Vec3 &v1 = vertices[0];
    const Vec3 &v2 = vertices[1];
    const Vec3 &v3 = vertices[2];
    
    const Vec2 &w1 = tcoords[0];
    const Vec2 &w2 = tcoords[1];
    const Vec2 &w3 = tcoords[2];
    
    real_t x1 = v2.x - v1.x;
    real_t x2 = v3.x - v1.x;
    real_t y1 = v2.y - v1.y;
    real_t y2 = v3.y - v1.y;
    real_t z1 = v2.z - v1.z;
    real_t z2 = v3.z - v1.z;
    
    real_t s1 = w2.x - w1.x;
    real_t s2 = w3.x - w1.x;
    real_t t1 = w2.y - w1.y;
    real_t t2 = w3.y - w1.y;
    
    real_t r = 1.0 / (s1 * t2 - s2 * t1);
    
    Vec3 sdir((t2 * x1 - t1 * x2) * r,
              (t2 * y1 - t1 * y2) * r,
              (t2 * z1 - t1 * z2) * r);
            
    Vec3 tdir((s1 * x2 - s2 * x1) * r,
              (s1 * y2 - s2 * y1) * r,
              (s1 * z2 - s2 * z1) * r);
    
    for (long a = 0; a < 3; a++)
    {      
    	Vec3 t;
    	real_t w;
    	  
        // Gram-Schmidt orthogonalize
        t = (sdir - normals[a] * normals[a].dot(sdir)).normalize();
        
        // Calculate handedness
        w = (normals[a].cross(sdir).dot(tdir) < 0.0) ? -1.0 : 1.0;
        
        tangents[a] = Vec4(t.x, t.y, t.z, w);
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
    : ambient_light(Vec3::Zero),
      start_time(0) {}

Scene::~Scene()
{
	for(SceneResourceList::iterator i=resources.begin();
	    i!=resources.end(); ++i)
	{
		delete *i;
	}
    
    for(RenderMethodList::iterator i=rendermethods.begin();
        i!=rendermethods.end(); ++i)
    {
		delete *i;
	}
}

