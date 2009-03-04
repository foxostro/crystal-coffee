/**
 * @file scene.h
 * @brief Class definitions of Material, Geometry, UpdatableGeometry,
 *  Camera, Light, and Scene.
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

#ifndef _SCENE_H_
#define _SCENE_H_

#include "rendermethod.h"
#include "vec/vec.h"
#include "vec/quat.h"
#include <string>
#include <vector>

class SceneResource
{
public:
	SceneResource() { /* Do Nothing */ }
	virtual ~SceneResource() { /* Do Nothing */ }
	virtual void init(void) { /* Do Nothing */ }
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
template<typename ELEMENT>
class BufferObject : public SceneResource {
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

/**
 * Represents a material property for a geometry or part of a geometry.
 */
class Material : public SceneResource
{
public:
    /**
     * P1 NOTE: the only items that must be dealt with are diffuse, phong,
     * ambient, and shininess. These correspond to diffuse, specular,
     * ambient, and shininess opengl material properties.
     */

    // the diffuse color
    Vec3 diffuse;

    // the phong specular color
    Vec3 phong;

    // the ambient color
    Vec3 ambient;

    // the specular reflection color (raytrace only)
    Vec3 specular;

    // the phong shininess
    real_t shininess;

    Material();
    virtual ~Material();

	void bind() const;

private:
    // no meaningful assignment or copy
    Material(const Material&);
    Material& operator=(const Material&);
};

/**
 * Represents a single texture unit and associated settings.
 */
class Texture : public SceneResource
{
public:
	Texture(const std::string &texture_name);

	virtual ~Texture();

	GLuint get_gltex_name() const {
		return gltex_name;
	}
	
	virtual void init(void) { load_texture(); }

private:
	void load_texture();
	
	// no meaningful assignment or copy
	Texture(const Texture &r);
	Texture& operator=(const Texture &r);

private:
	std::string texture_name;
	GLuint gltex_name;
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
    // Field of view of y-axis, in radians.
    real_t fov;
    // The aspect ratio.
    real_t aspect;
    // The near clipping plane.
    real_t near_clip;
    // The far clipping plane.
    real_t far_clip;
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

/**
 * The container class for information used to render a scene composed of
 * Geometries.
 */
class Scene
{
public:
    typedef std::vector<Light> LightList;
    typedef std::vector<SceneResource*> SceneResourceList;
    typedef std::vector<RenderMethod*> RenderMethodList;

    Camera camera;
    Vec3 ambient_light;
    LightList lights;
	SceneResourceList resources;
    RenderMethodList rendermethods;

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

private:
    // no meaningful assignment or copy
    Scene(const Scene&);
    Scene& operator=(const Scene&);
};

#endif /* _SCENE_H_ */

