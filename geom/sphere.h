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
	
	/** Subdivides a triangle on the icosphere, recursively.
	 *  @param v1 Triangle Vertex 1
	 *  @param v2 Triangle Vertex 2
	 *  @param v3 Triangle Vertex 3
	 *  @param depth Recursive depth subdivision
	 */
	static void subdivide(const Vec3 &v1,
                          const Vec3 &v2,
                          const Vec3 &v3,
                          int depth);
                          
	/** Generates spherical theta angles for a triangle.
	 *  The dicontinity of atan is handled so that these angles will be suitable
	 *  for direct conversion to texture coordinates.
	 *  @param v1 Triangle Vertex 1
	 *  @param v2 Triangle Vertex 2
	 *  @param v3 Triangle Vertex 3
	 *  @param theta1 Returns the theta of vertex 1 (radian longitude)
	 *  @param theta2 Returns the theta of vertex 2 (radian longitude)
	 *  @param theta3 Returns the theta of vertex 3 (radian longitude)
	 */
	static void texmap_theta(const Vec3 &v1,
                             const Vec3 &v2,
                             const Vec3 &v3,
                             real_t &theta1,
                             real_t &theta2,
                             real_t &theta3);
        
    static Vec3 vertices[];
	static unsigned int display_list;
};

#endif

