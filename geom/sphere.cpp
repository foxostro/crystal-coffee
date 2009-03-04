/**
 * @file sphere.cpp
 * @brief Sphere generator
 *
 * @author Kristin Siu (kasiu)
 * @author Eric Butler (edbutler)
 * @author Andrew Fox (arfox)
 */

#include "vec/mat.h"
#include "sphere.h"
#include "glheaders.h"
#include <memory.h>
#include <iostream>

/** Subdivides a triangle on the icosphere, recursively.
 *  @param v1 Triangle Vertex 1
 *  @param v2 Triangle Vertex 2
 *  @param v3 Triangle Vertex 3
 *  @param depth Recursive depth subdivision
 */
static void subdivide(std::vector<Face> &faces,
                      const Vec3 &v1,
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

static const Vec3 vertices[24] =
{
	Vec3( 1, 0,  1).normalize(), // Top, East, 3
	Vec3( 1, 0, -1).normalize(), // Top, East, 2
	Vec3( 0, 1,  0).normalize(), // Top, East, 1

	Vec3(-1, 0,  1).normalize(), // Top, North, 3
	Vec3( 1, 0,  1).normalize(), // Top, North, 2
	Vec3( 0, 1,  0).normalize(), // Top, North, 1

	Vec3(-1, 0, -1).normalize(), // Top, West, 3
	Vec3(-1, 0,  1).normalize(), // Top, West, 2
	Vec3( 0, 1,  0).normalize(), // Top, West, 1
	
	Vec3( 1, 0, -1).normalize(), // Top, South, 3
	Vec3(-1, 0, -1).normalize(), // Top, South, 2
	Vec3( 0, 1,  0).normalize(), // Top, South, 1
	
	Vec3( 0, -1,  0).normalize(), // Bottom, East, 1
	Vec3( 1,  0, -1).normalize(), // Bottom, East, 2
	Vec3( 1,  0,  1).normalize(), // Bottom, East, 3

	Vec3( 0, -1,  0).normalize(), // Bottom, North, 1
	Vec3( 1,  0,  1).normalize(), // Bottom, North, 2
	Vec3(-1,  0,  1).normalize(), // Bottom, North, 3

	Vec3( 0, -1,  0).normalize(), // Bottom, West, 1
	Vec3(-1,  0,  1).normalize(), // Bottom, West, 2
	Vec3(-1,  0, -1).normalize(), // Bottom, West, 3
	
	Vec3( 0, -1,  0).normalize(), // Bottom, South, 1
	Vec3(-1,  0, -1).normalize(), // Bottom, South, 2
	Vec3( 1,  0, -1).normalize(), // Bottom, South, 3
};

TriangleSoup gen_sphere(int num_of_divisions)
{
	std::vector<Face> faces;
	
	for(int i = 0; i < 24; i+=3)
	{
		subdivide(faces,
		          vertices[i+0],
		          vertices[i+1],
		          vertices[i+2],
		          num_of_divisions);
	}
	
	return TriangleSoup(faces);
}
               
static void texmap_theta(const Vec3 &v1,
                         const Vec3 &v2,
                         const Vec3 &v3,
                         real_t &theta1,
                         real_t &theta2,
                         real_t &theta3)
{
	theta1 = atan2(v1.z, v1.x);
	theta2 = atan2(v2.z, v2.x);
	theta3 = atan2(v3.z, v3.x);
	
	/* There atan function has a range from [-pi, pi]. Some triangles will have
	 * vertices that span the transition between -pi and 0, or between pi and
	 * -pi. This special case code ensures that no triangle in the sphere is 
	 * mapped incorrectly due to the incontinuity.
	 */
		
	if(theta1 - theta2 > PI)
	{
		theta1 -= 2 * PI;
	}
	else if(theta1 - theta3 > PI)
	{
		theta3 += 2 * PI;
	}
	else if(theta2 - theta1 > PI)
	{
		theta1 += 2 * PI;
	}
	else if(theta2 - theta3 > PI)
	{
		theta3 += 2 * PI;
	}
	else if(theta3 - theta1 > PI)
	{
		theta3 -= 2 * PI;
	}
	else if(theta3 - theta2 > PI)
	{
		theta3 -= 2 * PI;
	}

	if(theta1 - theta2 < -PI)
	{
		theta1 += 2 * PI;
	}
	else if(theta1 - theta3 < -PI)
	{
		// fix so that sphere suffers less distortion near the poles
		if(fabs(theta1) < 0.001)
		{
			theta1 += PI;
		}
		else
		{
			theta3 -= 2 * PI;
		}
	}
	else if(theta2 - theta1 < -PI)
	{
		theta1 -= 2 * PI;
	}
	else if(theta2 - theta3 < -PI)
	{
		theta3 -= 2 * PI;
	}
	else if(theta3 - theta1 < -PI)
	{
		theta3 += 2 * PI;
	}
	else if(theta3 - theta2 < -PI)
	{
		theta3 += 2 * PI;
	}
}
   
static void subdivide(std::vector<Face> &faces,
                      const Vec3 &v1,
                      const Vec3 &v2,
                      const Vec3 &v3,
                      int depth)
{
	Vec3 v12, v23, v31;
	
	assert(depth>=0);

	if(depth == 0)
	{
		Face face;
		real_t theta1, theta2, theta3;
		
		face.vertices[0] = v1;
		face.vertices[1] = v2;
		face.vertices[2] = v3;
		
		face.normals[0] = v1;
		face.normals[1] = v2;
		face.normals[2] = v3;
						
		texmap_theta(v1, v2, v3, theta1, theta2, theta3);
		
		face.tcoords[0] = Vec2(0.5 - theta1 / (2 * PI), acos(-v1.y) / PI);
		face.tcoords[1] = Vec2(0.5 - theta2 / (2 * PI), acos(-v2.y) / PI);
		face.tcoords[2] = Vec2(0.5 - theta3 / (2 * PI), acos(-v3.y) / PI);
		
		calculate_triangle_tangent(face.vertices, face.normals,
			                       face.tcoords, face.tangents);
			                       
		faces.push_back(face);
						
		return;
	}

	v12 = v1 + v2;
	v23 = v2 + v3;
	v31 = v3 + v1;
	v12.normalize();
	v23.normalize();
	v31.normalize();

	subdivide(faces, v1,  v12, v31, depth-1);
	subdivide(faces, v2,  v23, v12, depth-1);
	subdivide(faces, v3,  v31, v23, depth-1);
	subdivide(faces, v12, v23, v31, depth-1);
}

