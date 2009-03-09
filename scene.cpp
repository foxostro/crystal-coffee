/**
 * @file scene.cpp
 * @brief Function definitions for the Material, Geometry, UpdatableGeometry,
 *  Camera, Light, and Scene classes.
 *
 * @author Eric Butler (edbutler)
 * @author Kristin Siu (kasiu)
 * @author Andrew Fox (arfox)
 */

#include <SDL/SDL.h>
#include <iostream>
#include "vec/mat.h"
#include "scene.h"
#include "glheaders.h"
#include "devil_wrapper.h"

char* ShaderProgram::load_file(const char* file)
{
	std::ifstream infile;
	infile.open(file);

	if(infile.fail()){
		fprintf(stderr, "ERROR: cannot open file %s\n", file);
		infile.close();
		exit(2);
	}

	infile.seekg(0, std::ios::end );
	int length = infile.tellg();
	infile.seekg(0, std::ios::beg );

	char* buffer = new char[length];
	infile.getline(buffer, length, '\0');

	infile.close();

	return buffer;
}

/**
* Load a file as either a vertex shader or a fragment shader, and attach
* it to a program 
* @param file  The file to load
* @param type  Either GL_VERTEX_SHADER_ARB, or GL_FRAGMENT_SHADER_ARB
* @param program  The shading program to which the shaders are attached
*/
void ShaderProgram::load_shader(const char* file,
								GLint type,
								GLhandleARB& program)
{    
	int result;
	char error_msg[1024];

	const char* src = load_file(file);
	// Create shader object
	GLhandleARB shader = glCreateShaderObjectARB(type);

	// Load Shader Sources
	glShaderSourceARB(shader, 1, &src, NULL);
	// Compile The Shaders
	glCompileShaderARB(shader);
	// Get compile result
	glGetObjectParameterivARB(shader, GL_OBJECT_COMPILE_STATUS_ARB, &result);
	if(!result){
		glGetInfoLogARB(shader, sizeof(error_msg), NULL, error_msg);
		fprintf(stderr, "GLSL COMPILE ERROR(%s): %s\n", file, error_msg);
		exit(2);
	}

	// Attach The Shader Objects To The Program Object
	glAttachObjectARB(program, shader);
}

/**
* Creates a program, loads the given shader programs into it, and returns it.
*/  
GLhandleARB ShaderProgram::load_shaders(const char* vert_file,
										const char* frag_file)
{
	// Create shader program
	GLhandleARB program  = glCreateProgramObjectARB();

	// Load vertex shader
	std::cout << "loading vertex shader " << vert_file << std::endl;
	load_shader(vert_file, GL_VERTEX_SHADER_ARB, program);

	// Load fragment shader
	std::cout << "loading fragment shader " << frag_file << std::endl;
	load_shader(frag_file, GL_FRAGMENT_SHADER_ARB, program);

	glLinkProgramARB(program);

	return program;
}

ShaderProgram::ShaderProgram(const char* _vert_file, const char* _frag_file)
: program(0),
  vert_file(_vert_file),
  frag_file(_frag_file)
{
	program = load_shaders(vert_file, frag_file);
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgramsARB(1, &program);
}

void ShaderProgram::init()
{
	/* Do Nothing */
}

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
  ambient(Vec3::Ones),
  specular(Vec3::Ones),
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
	const GLfloat c_a[] = { (GLfloat)ambient.x, (GLfloat)ambient.y, (GLfloat)ambient.z, 1 };
	const GLfloat c_d[] = { (GLfloat)diffuse.x, (GLfloat)diffuse.y, (GLfloat)diffuse.z, 1 };
	const GLfloat c_s[] = { (GLfloat)specular.x, (GLfloat)specular.y, (GLfloat)specular.z, 1 };
	const GLfloat black[] = { 0, 0, 0, 1 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, c_a);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, c_d);
	glMaterialfv(GL_FRONT, GL_SPECULAR, c_s);
	glMaterialfv(GL_FRONT, GL_EMISSION, black);
	glMaterialf(GL_FRONT, GL_SHININESS, (GLfloat)shininess);
}

Texture::~Texture()
{
	glDeleteTextures(1, &gltex_name);
}

void Texture::load_texture()
{
	// don't load texture if already loaded or filename is blank
	if(!gltex_name && !texture_name.empty()) {
		std::clog << "loading texture " << texture_name << std::endl;
		gltex_name = ilutGLLoadImage(const_cast<char*>(texture_name.c_str()));
	}
}

void Texture::bind( void ) const
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, get_gltex_name());
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
    : position(Vec3::Zero), orientation(Quat::Identity), focus_dist(1) {}

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
: position(Vec3::Zero),
  color(Vec3::Ones),
  intensity(1)
{
	// Do Nothing
}



Scene::Scene()
: ambient_light(Vec3::Zero),
  start_time(0),
  primary_camera(NULL)
{
	// Do Nothing
}

Scene::~Scene()
{
	for(SceneResourceList::iterator  i=resources.begin();     i!=resources.end();     ++i) delete *i;    
	for(RenderMethodList::iterator   i=rendermethods.begin(); i!=rendermethods.end(); ++i) delete *i;
	for(TickableList::iterator       i=tickables.begin();     i!=tickables.end();     ++i) delete *i;
	for(PassList::iterator           i = passes.begin();      i!=passes.end();        ++i) delete *i;
}

Pass::Pass(void)
 : rendertarget(0)
{
	clear_color = Vec4(0.0, 0.0, 0.0, 1.0);
}

Pass::~Pass()
{
	for(RenderInstanceList::iterator i = instances.begin(); i != instances.end(); ++i)
	{
		delete *i;
	}
}

void Pass::set_camera(void)
{
	GLdouble eyex = camera.get_position().x;
	GLdouble eyey = camera.get_position().y;
	GLdouble eyez = camera.get_position().z;
	GLdouble centerx = camera.get_position().x + camera.get_direction().x * camera.focus_dist;
	GLdouble centery = camera.get_position().y + camera.get_direction().y * camera.focus_dist;
	GLdouble centerz = camera.get_position().z + camera.get_direction().z * camera.focus_dist;
	GLdouble upx = camera.get_up().x;
	GLdouble upy = camera.get_up().y;
	GLdouble upz = camera.get_up().z;

	// set the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixd(proj.m);

	// set the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyex, eyey, eyez,
	          centerx, centery, centerz,
	          upx, upy, upz);
}

void Pass::set_light_positions(const LightList & lights)
{
	for(int i=0; i<8 && i < (int)lights.size(); ++i)
	{
		const Light & light = lights[i];
		const GLfloat position[] = { (GLfloat)light.position.x,
		                             (GLfloat)light.position.y,
		                             (GLfloat)light.position.z, 1 };
		glLightfv(GL_LIGHT0 + i, GL_POSITION, position);
	}
}

void Scene::render()
{
	for(PassList::iterator i = passes.begin();
		i != passes.end(); ++i)
	{
		(*i)->render(this);
	}

	SDL_GL_SwapBuffers();
}

RenderTarget::~RenderTarget()
{
	glDeleteFramebuffersEXT(1, &fbo);
	glDeleteRenderbuffersEXT(1, &renderbuffer);
}

RenderTarget::RenderTarget( const ivec2 &_dimensions ) : fbo(0), renderbuffer(0), dimensions(_dimensions)
{
	// Do Nothing
}

void RenderTarget::init( void )
{
	glGenFramebuffersEXT(1, &fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

	glGenRenderbuffersEXT(1, &renderbuffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbuffer);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, dimensions.x, dimensions.y); 
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, renderbuffer);

	glGenTextures(1, &gltex_name);
	glBindTexture(GL_TEXTURE_2D, gltex_name);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  dimensions.x, dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, gltex_name, 0);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// error checking
	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(status != GL_FRAMEBUFFER_COMPLETE_EXT) {
		std::cerr << "ERROR: Failed to create render-target" << std::endl;
	}
	CHECK_GL_ERROR();
}

void RenderTarget::bind() const
{
	CHECK_GL_ERROR();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

	// No need to save state. The viewport is reset by the next pass anyway.
	glViewport(0, 0, dimensions.x, dimensions.y);
}

void RenderTarget::unbind()
{
	CHECK_GL_ERROR();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

CubeMapRenderTarget::CubeMapRenderTarget(const ivec2 &_dimensions)
	: RenderTarget(_dimensions)
{
	assert(!"stub");
}

void CubeMapRenderTarget::init( void )
{
	assert(!"stub");
}

void CubeMapRenderTarget::bind() const
{
	assert(!"bind is n/a to the  Cube Map RenderTarget");
}

void CubeMapRenderTarget::bind( int face ) const
{
	assert(!"stub");
}

GLenum CubeMapTexture::face_targets[6] =
{
	GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT
};

CubeMapTexture::CubeMapTexture(const std::string &face1,
							   const std::string &face2,
							   const std::string &face3,
							   const std::string &face4,
							   const std::string &face5,
							   const std::string &face6)
{
	texture_name_face[0] = face1;
	texture_name_face[1] = face2;
	texture_name_face[2] = face3;
	texture_name_face[3] = face4;
	texture_name_face[4] = face5;
	texture_name_face[5] = face6;
}

void CubeMapTexture::load_face(GLenum target, const std::string &filename)
{
	ILuint ImageName, width, height, bpp;
	ILubyte *data;
	GLint internalformat;
	GLenum format;

	ilGenImages(1, &ImageName);
	ilBindImage(ImageName);
	ilLoadImage(filename.c_str());

	width = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);
	bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	data = ilGetData(); 

	CHECK_IL_ERROR();

	assert(bpp == 3 || bpp == 4);

	internalformat = (bpp==4) ? GL_RGBA8 : GL_RGB8;
	format = (bpp==4) ? GL_RGBA : GL_RGB;

	glTexImage2D(target,
	             0,
				 internalformat,
	             width,
				 height,
				 0,
				 format,
				 GL_UNSIGNED_BYTE,
				 data);

	ilDeleteImages(1, &ImageName); 

	CHECK_GL_ERROR();
}

void CubeMapTexture::init(void)
{
	if(gltex_name)
		return;

	glGenTextures(1, &gltex_name);

	glEnable(GL_TEXTURE_CUBE_MAP_EXT);
	glBindTexture(GL_TEXTURE_CUBE_MAP_EXT, gltex_name);

	for(int i=0; i<6; i++)
	{
		load_face(face_targets[i], texture_name_face[i]);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP);

	CHECK_GL_ERROR();
}

void CubeMapTexture::bind_cubemap() const
{
	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_TEXTURE_CUBE_MAP_EXT);
	glBindTexture(GL_TEXTURE_CUBE_MAP_EXT, gltex_name);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
}
