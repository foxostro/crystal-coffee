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
	static void init_sphere();
	
    Sphere();
    Sphere(const Vec3& pos, const Quat& ori, const Vec3& scl,
           real_t rad, Material* mat, Effect* efc=0);
    virtual ~Sphere();

    // the radius of this sphere.
    real_t radius;

    virtual void draw() const;
    
private:
	/** Draws a unit isosphere.
	  * @param num_of_divisions Number of times to subdivide the icosphere.
	  */
	static void draw_ico_sphere(int num_of_divisions);
	static void subdivide(const Vec3 &v1,
                          const Vec3 &v2,
                          const Vec3 &v3,
                          const Vec2 &st1,
                          const Vec2 &st2,
                          const Vec2 &st3,
                          int depth);
        
    static Vec3 vertices[];
    static Vec2 texcoords[];
	static unsigned int display_list;
};

#endif

