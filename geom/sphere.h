/**
 * @file sphere.h
 * @brief Class defnition for Sphere.
 *
 * @author Kristin Siu (kasiu)
 * @author Eric Butler (edbutler)
 */

/*
    EDIT THIS FILE FOR P1. However, do not change existing constructor
    signatures. The staff scene loader requires all of those to be intact.
 */

#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "scene.h"

class Sphere
{
public:
	Sphere() : vertices_buffer(0),
	           normals_buffer(0),
	           tangents_buffer(0),
	           tcoords_buffer(0) {}
	
	BufferObject<Vec3> * vertices_buffer;
	BufferObject<Vec3> * normals_buffer;
	BufferObject<Vec4> * tangents_buffer;
	BufferObject<Vec2> * tcoords_buffer;
};

Sphere gen_sphere(int num_of_divisions);

#endif

