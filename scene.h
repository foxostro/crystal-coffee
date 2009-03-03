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

/**
 * Represents a material property for a geometry or part of a geometry.
 */
class Material
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
    ~Material();

	void bind() const;

private:
    // no meaningful assignment or copy
    Material(const Material&);
    Material& operator=(const Material&);
};

/**
 * Represents a single texture unit and associated settings.
 */
class Texture
{
public:
	Texture(const std::string &texture_name);

	~Texture();

	GLuint get_gltex_name() const {
		return gltex_name;
	}

	void load_texture();

private:
	// no meaningful assignment or copy
	Texture(const Texture &r);
	Texture& operator=(const Texture &r);

private:
	std::string texture_name;
	GLuint gltex_name;
};

class Geometry // TODO: Replace with some kind of VertexStream class
{
public:
    Geometry();
    Geometry(const Vec3& pos, const Quat& ori, const Vec3& scl);
    virtual ~Geometry();

    /*
       World transformation are applied in the following order:
       1. Scale
       2. Orientation
       3. Position
    */

    // The world position of the object.
    Vec3 position;
    // The world orientation of the object.
    Quat orientation;
    // The world scale of the object.
    Vec3 scale;

	virtual void draw() const = 0; // TODO: Remove me
   
	/** Sets the object's transformation */
	void set_transformation() const;
	
	/** Calculate the tangents for one triangle */
	static void CalculateTriangleTangent(const Vec3 *vertices,
                                         const Vec3 *normals,
	                                     const Vec2 *tcoords,
	                                     Vec4 *tangents);

};

class UpdatableGeometry : public Geometry
{
public:
    UpdatableGeometry() { /* Do Nothing */ }

    UpdatableGeometry(const Vec3& pos,
	                  const Quat& ori,
					  const Vec3& scl)
        : Geometry(pos, ori, scl) { /* Do Nothing */ }

    virtual ~UpdatableGeometry() { /* Do Nothing */ }

    /**
     * Updates this Geometry to the given time.
     * @param time The absolute world time.
     */
    virtual void update(real_t time) = 0;
};

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
    typedef std::vector<Geometry*> GeometryList;
    typedef std::vector<UpdatableGeometry*> UpdatableGeometryList;
	typedef std::vector<Material*> MaterialList;
	typedef std::vector<Texture*> TextureList;
    typedef std::vector<RenderMethod*> EffectList;

    // the camera
    Camera camera;

    // the amibient light of the scene
    Vec3 ambient_light;

    // the absolute time at which to start updates for ths scene.
    real_t start_time;

    // list of all lights in the scene
    LightList lights;

    // list of all geometry (includes updatable geometry). deleted in dctor
    GeometryList geoms;

    // list of all updatable geometry
    UpdatableGeometryList updatable_objects;

    // list of all materials used by the objects/effects. deleted in dctor
    MaterialList materials;

	// list of all textures used by the scene. deleted in dctor
	TextureList textures;

    // list of all effects used by objects. deleted in deconstructor
    // (USED STARTING P2)
    EffectList render_methods;

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

