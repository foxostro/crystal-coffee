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

class Triangle
{
public:
    // construct a triangle with different materials/normals at each vertex
    Triangle(const Vec3 vertices[3],
             const Vec2 tcoords[3],
             const Vec3 normals[3]);
    
    BufferObject<Vec4> tangents_buffer;
    BufferObject<Vec3> normals_buffer;
    BufferObject<Vec3> vertices_buffer;
    BufferObject<Vec2> tcoords_buffer;
    
private:
	void calculate_tangents();
};

#endif

