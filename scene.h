/**
 * @file scene.h
 * @brief Class definitions of Material, Geometry, UpdatableGeometry,
 *  Camera, Light, and Scene.
 *
 * @author Eric Butler (edbutler)
 * @author Kristin Siu (kasiu)
 * @author Andrew Fox (arfox)
 */

#ifndef _SCENE_H_
#define _SCENE_H_

#include "rendermethod.h"
#include "vec/vec.h"
#include "vec/quat.h"
#include "material.h"
#include <string>
#include <vector>
#include <list>
#include <boost/shared_ptr.hpp>

class Tickable
{
public:
	virtual ~Tickable() { /* Do Nothing */ }
	virtual void tick(real_t time) = 0;

protected:
	Tickable()  { /* Do Nothing */ }
};

struct Face
{
	Vec3 vertices[3];
	Vec3 normals[3];
	Vec4 tangents[3];
	Vec2 tcoords[3];
};

class SceneResource
{
public:
	virtual void init(void) { /* Do Nothing */ }
	virtual ~SceneResource() { /* Do Nothing */ }

protected:
	SceneResource() { /* Do Nothing */ }
};

class RenderInstance
{
public:
	~RenderInstance(void) { /* Do Nothing */ }

	RenderInstance(const Mat4 &_transform, const boost::shared_ptr<RenderMethod> _rendermethod)
		: transform(_transform),
		  rendermethod(_rendermethod)
	{
		assert(rendermethod);
	}

	void draw(void) const
	{
		assert(rendermethod);
		rendermethod->draw(transform);
	}

private:
	const Mat4 transform;
	const boost::shared_ptr<RenderMethod> rendermethod;
};

class ShaderProgram : public SceneResource
{
public:
	ShaderProgram(const char* vert_file, const char* frag_file);

	virtual ~ShaderProgram();

	virtual void init();

	inline GLhandleARB get_program() const
	{
		return program;
	}

private:
	char* load_file(const char* file);
	void load_shader(const char* file, GLint type, GLhandleARB& program);
	GLhandleARB load_shaders(const char* vert_file, const char* frag_file);

private:
	GLhandleARB program;
	const char* vert_file;
	const char* frag_file;
};

enum BUFFER_USAGE {
	STREAM_DRAW,
	STREAM_READ,
	STREAM_COPY,
	STATIC_DRAW,
	STATIC_READ,
	STATIC_COPY,
	DYNAMIC_DRAW,
	DYNAMIC_READ,
	DYNAMIC_COPY
};

/**
Contains a buffer of graphically related data such as an index array or a
vertex array. This data may be stored in memory on the graphics device after
being submitted.
*/
template<typename ELEMENT> class BufferObject {
public:
	/** Destructor */
	virtual ~BufferObject();
	
	/** Default Constructor */
	BufferObject();
	
	/**
	Constructor
	@param numElements Number of elements in the buffer
	@param buffer Readable on the client-side.  Buffer is copied here and
	no ownership is passed. If null is passed for the 'buffer' parameter
	then a buffer is allocated internally (accessible through lock) but
	it will be uninitialized upon construction.
	*/
	BufferObject(int numElements, const ELEMENT *buffer);
	
	/**
	Copy constructor. Copies the contents of a given buffer.
	@param copyMe Buffer to clone
	*/
	BufferObject(const BufferObject &copyMe);
	
	/**
	Creates a copy of this buffer.
	The copy is created on the heap with new, and it is the responsibility of
	the caller to free it.
	@return New object with copy of this buffer
	*/
	BufferObject<ELEMENT> * clone(void) const;

	void create(int numElements, BUFFER_USAGE usage)
	{
		recreate(numElements, NULL, usage);
	}

	void create(int numElements, const ELEMENT *buffer, BUFFER_USAGE usage)
	{
		recreate(numElements, buffer, usage);
	}
	
	/**
	Reallocates memory for the buffer
	@param numElements Number of elements in the buffer.  Must be greater
	than zero unless both numElements=0 and buffer=0
	@param buffer Readable on the client-side.  Buffer is copied here and
	no ownership is passed. If null is passed for the 'buffer' parameter
	then a buffer is allocated internally (accessible through lock) but
	it will be uninitialized upon construction.
	@param usage Enumerant describing how the buffer will be used.  This
	is a hint to the graphics driver as to how the buffer should be stored
	in memory.
	*/
	void recreate(int numElements, const ELEMENT *buffer, BUFFER_USAGE usage);
	
	/**
	Gets the number of elements in the buffer
	@return Number of elements in the buffer
	*/
	int getNumber() const;
	
	/** Binds the buffer for use on the GPU */
	void bind() const;
	
	/**
	Locks the buffer to allow read-write access by the client.
	@return elements array
	*/
	ELEMENT* lock();
	
	/**
	Obtaind read access to the buffer. Do not rely on write access.
	@return elements array
	*/
	ELEMENT* read_lock();
	
	/**
	Unlocks the buffer and removes memory maps.
	Only call on locked buffers.
	*/
	void unlock() const;
	
private:
	void create_cpu_buffer(int numElements, const ELEMENT * buffer);
	
	void create_gpu_buffer(int numElements,
	                       const ELEMENT * buffer,
	                       GLenum usage);
	                       
	static GLenum getTarget();
	
	static GLenum getGLUsageToken(BUFFER_USAGE usage);
	
private:
	/** Indicates that the buffer is currently locked */
	mutable bool locked;
	
	/** Number of elements in the buffer */
	int numElements;
	
	/** Buffer, stored on the client-side */
	ELEMENT *buffer;
	
	/** OpenGL buffer object name */
	GLuint handle;
	
	/** Store this so if we are cloned, the copy can set usage properly */
	BUFFER_USAGE usage;
};

/** Represents a single texture unit and associated settings. */
class Texture : public SceneResource
{
public:
	virtual ~Texture();
	Texture(void) : gltex_name(0) {}

	inline GLuint get_gltex_name() const { return gltex_name; }

	virtual void init(void) = 0;
	virtual void bind(void) const = 0;

private:
	// no meaningful assignment or copy
	Texture(const Texture &r);
	Texture& operator=(const Texture &r);

public:
	std::string texture_name;

protected:
	GLuint gltex_name;
};

class Texture2D : public Texture
{
public:
	Texture2D(void) {}
	Texture2D(const std::string &f) : texture_name(f) {}

	virtual void init(void) { load_texture(); }
	virtual void bind(void) const;

private:
	// no meaningful assignment or copy
	Texture2D(const Texture2D &r);
	Texture2D& operator=(const Texture2D &r);

	void load_texture();

public:
	std::string texture_name;
};

class CubeMapTexture : public Texture
{
public:
	CubeMapTexture(void) { /* Do Nothing */ }

	CubeMapTexture(const std::string &face1,
	               const std::string &face2,
				   const std::string &face3,
				   const std::string &face4,
				   const std::string &face5,
				   const std::string &face6);

	virtual void init(void);

	virtual void bind(void) const
	{
		bind_cubemap();
	}

private:
	// no meaningful assignment or copy
	CubeMapTexture(const CubeMapTexture &r);
	CubeMapTexture& operator=(const CubeMapTexture &r);

	void load_face(GLenum target, const std::string &filename);

	void init_blank_face(GLenum target, const ivec2 &dimensions);

	void bind_cubemap() const;

public:
	std::string texture_name_face[6];
};

extern GLenum face_targets[6];
extern Quat face_orientation[6];

class RenderTarget2D : public Texture2D
{
public:
	virtual ~RenderTarget2D();

	RenderTarget2D(const ivec2 &_dimensions);

	virtual void init();

	virtual void bind_render_target() const;

private:
	// no meaningful assignment or copy
	RenderTarget2D(const RenderTarget2D &r);
	RenderTarget2D& operator=(const RenderTarget2D &r);

private:
	GLuint fbo;
	GLuint renderbuffer;
	ivec2 dimensions;
};

class CubeMapTarget : public CubeMapTexture
{
public:
	CubeMapTarget(const ivec2 &_dimensions);
	virtual void init(void);
	virtual void bind_render_target(int face) const;

private:
	// no meaningful assignment or copy
	CubeMapTarget(const CubeMapTarget &r);
	CubeMapTarget& operator=(const CubeMapTarget &r);

	void init_face_texture(GLenum target, const ivec2 &dimensions);
	void create_cubemap_texture(const ivec2 &dimensions);

private:
	GLuint fbo;
	GLuint renderbuffer;
	ivec2 dimensions;
};

/** Calculate the tangents for one triangle */
void calculate_triangle_tangent(const Vec3 *vertices,
                                const Vec3 *normals,
	                            const Vec2 *tcoords,
	                            Vec4 *tangents);

/**
 * Stores position data of the camera.
 */
class Camera
{
public:
    Camera();

    // accessor functions

    // Returns the world position.
    const Vec3& get_position() const;
    // Returns the direction vector, a unit vector pointing in the direction
    // the camera is facing.
    Vec3 get_direction() const;
    // Returns the up vector, a unit vector pointing in the direction up from
    // the camera's orientation.
    Vec3 get_up() const;
    // Returns the field of view in radians.
    real_t get_fov_radians() const;
    // Returns the field of view in degrees.
    real_t get_fov_degrees() const;
    // Returns the aspect ratio (width/height).
    real_t get_aspect_ratio() const;
    // Returns the distance from the camera to the near clipping plane.
    real_t get_near_clip() const;
    // Returns the distance from the camera to the far clipping plane.
    real_t get_far_clip() const;

    // mutator functions

    // translates position by v
    void translate(const Vec3& v);
    // rotates about the X axis
    void pitch(real_t radians);
    // rotates about the Z axis
    void roll(real_t radians);
    // rotates about the Y axis
    void yaw(real_t radians);
    // rotates about the given axis
    void rotate(const Vec3& axis, real_t radians);
    // swings camera around focal point along X axis
    void pitch_about_focus(real_t radians);
    // swings camera around focal point along Y axis
    void yaw_about_focus(real_t radians);
    // swings camera around focal point along given axis
    void rotate_about_focus(const Vec3& axis, real_t radians);

    // members

    // The world position of the camera.
    Vec3 position;
    // The orientation of the camera, relative to a default direction
    // of negative z axis and default up vector of y axis.
    Quat orientation;
    // Distance to the point about which the camera's rotate functions operate.
    real_t focus_dist;
};

class Light
{
public:
    Light();

    // The position of the light, relative to world origin.
    Vec3 position;
    // The color of the light (both diffuse and specular)
    Vec3 color;
    // Total intensity of this light. (USED STARTING P4)
    real_t intensity;
};

typedef std::vector<Light> LightList;
class Scene;

class Pass
{
public:
	Mat4 proj;
	Camera camera;
	typedef std::vector<  boost::shared_ptr<RenderInstance> > RenderInstanceList;
	RenderInstanceList instances;
	Vec4 clear_color;

public:
	virtual ~Pass();

	virtual void render(const Scene * scene) = 0;

protected:
	Pass(void);
	void set_camera(void);
	void set_light_positions(const LightList & lights);
};

class Scene
{
public:
	typedef std::vector< boost::shared_ptr<SceneResource> > SceneResourceList;
	typedef std::vector< boost::shared_ptr<RenderMethod> > RenderMethodList;
	typedef std::vector< boost::shared_ptr<Tickable> > TickableList;
	typedef std::list< boost::shared_ptr<Pass> > PassList;

	Camera * primary_camera;
    Vec3 ambient_light;
    LightList lights;
	SceneResourceList resources;
	RenderMethodList rendermethods;
	TickableList tickables;
	PassList passes;

    // the absolute time at which to start updates for ths scene.
    real_t start_time;

    /**
     * Creates a new empty scene.
     */
    Scene();

    /**
     * Destroys this scene. Deletes all objects in materials and objects.
     * Assumes that all objects in updatable_objects are also in objects.
     */
    ~Scene();

	void render();

private:
    // no meaningful assignment or copy
    Scene(const Scene&);
    Scene& operator=(const Scene&);
};

#endif /* _SCENE_H_ */

