/**
 * @file triangle.h
 * @brief Class definition for Triangle.
 *
 * @author Eric Butler (edbutler)
 */

/*
    EDIT THIS FILE FOR P1. However, do not change existing constructor
    signatures. The staff scene loader requires all of those to be intact.
 */

#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "scene.h"

class Triangle : public Geometry
{
public:
    Triangle();

    // construct a triangle with different materials/normals at each vertex
    Triangle(const Vec3& pos, const Quat& ori, const Vec3& scl,
             const Vec3 vertices[3], const Vec2 tcoords[3],
             const Vec3 normals[3], Material* materials[3], Effect* efc=0);

    // construct a triangle with same material/normal at each vertex
    Triangle(const Vec3& pos, const Quat& ori, const Vec3& scl,
             const Vec3 vertices[3], const Vec2 tcoords[3],
             const Vec3& normal, Material* material, Effect* efc=0);

    virtual ~Triangle();

    // the vertices of this triangle (in local space).
    Vec3 vertices[3];
    // the texture coordinates at each vertex.
    Vec2 tcoords[3];
    // the normals of each vertex (in local space).
    Vec3 normals[3];
    // the tangents at each vertex (in local space)
    Vec3 tangents[3];
    // the material at each vertex (only applies to raytracing).
    Material* materials[3];

    virtual void draw() const;
    
private:
	void calculate_tangents();
};

#endif

