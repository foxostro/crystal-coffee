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

class Sphere : public Geometry
{
public:
    Sphere();
    Sphere(const Vec3& pos, const Quat& ori, const Vec3& scl,
           real_t rad, Material* mat, Effect* efc=0);
    virtual ~Sphere();

    // the radius of this sphere.
    real_t radius;

    virtual void draw() const;
};

#endif

