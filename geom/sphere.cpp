/**
 * @file sphere.cpp
 * @brief Function defnitions for the Sphere class.
 *
 * @author Kristin Siu (kasiu)
 * @author Eric Butler (edbutler)
 */

/*
    EDIT THIS FILE FOR P1. However, do not change existing constructor
    signatures. The staff scene loader requires all of those to be intact.
 */

#include "vec/mat.h"
#include "sphere.h"
#include "glheaders.h"

Vec3 Sphere::vertices[24] =
{
	Vec3( 0, 1,  0).normalize(), // Top, East, 1
	Vec3( 1, 0, -1).normalize(), // Top, East, 2
	Vec3( 1, 0,  1).normalize(), // Top, East, 3

	Vec3( 0, 1,  0).normalize(), // Top, North, 1
	Vec3( 1, 0,  1).normalize(), // Top, North, 2
	Vec3(-1, 0,  1).normalize(), // Top, North, 3

	Vec3( 0, 1,  0).normalize(), // Top, West, 1
	Vec3(-1, 0,  1).normalize(), // Top, West, 2
	Vec3(-1, 0, -1).normalize(), // Top, West, 3
	
	Vec3( 0, 1,  0).normalize(), // Top, South, 1
	Vec3(-1, 0, -1).normalize(), // Top, South, 2
	Vec3( 1, 0, -1).normalize(), // Top, South, 3
	
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

void Sphere::init_sphere()
{
	display_list = glGenLists(1);
	glNewList(display_list, GL_COMPILE);
	draw_ico_sphere(3);
	glEndList();
}

Sphere::Sphere()
: radius(0),
  display_list(0)
{
	init_sphere();
}

Sphere::Sphere(const Vec3& pos, const Quat& ori, const Vec3& scl,
               real_t rad, Material* mat, Effect* efc)
: Geometry(pos, ori, scl, mat, efc),
  radius(rad),
  display_list(0)
{
	init_sphere();
}

Sphere::~Sphere()
{
	glDeleteLists(display_list, 1);
}
               
void Sphere::texmap_theta(const Vec3 &v1,
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
		// fix so that draw_ico_sphere(1) suffers less distortion near the poles
		if(abs(theta1) < 0.001)
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
   
void Sphere::subdivide(const Vec3 &v1,
                       const Vec3 &v2,
                       const Vec3 &v3,
                       int depth)
{
	Vec3 v12, v23, v31;
	
	assert(depth>=0);

	if(depth == 0)
	{
		Vec2 st1, st2, st3;
		real_t theta1, theta2, theta3, phi1, phi2, phi3;
						
		texmap_theta(v1, v2, v3, theta1, theta2, theta3);
				
		phi1 = acos(-v1.y);
		phi2 = acos(-v2.y);
		phi3 = acos(-v3.y);
		
		st1 = Vec2(theta1 / (2 * PI), phi1 / PI);
		st2 = Vec2(theta2 / (2 * PI), phi2 / PI);
		st3 = Vec2(theta3 / (2 * PI), phi3 / PI);
		
/*****************************************************************************/
		
		Vec4 tangents[3];
		Vec3 vertices[3] = { v1, v2, v3 };
		Vec3 normals[3] = { v1, v2, v3 };
		Vec2 tcoords[3] = { st1, st2, st3 };
		
		if(effect && effect->areTangentsRequired())
		{
			CalculateTriangleTangent(vertices, normals, tcoords, tangents);
		}
		
		for(int i=0; i<3; ++i)
		{
			if(effect && effect->areTangentsRequired())
			{				
				glVertexAttrib4dARB(effect->getTangentAttribSlot(),
							        tangents[i].x,
							        tangents[i].y,
							        tangents[i].z,
							        tangents[i].w);
			}
		
			glTexCoord2d(tcoords[i].x, tcoords[i].y);
			glNormal3f(normals[i].x, normals[i].y, normals[i].z);
			glVertex3d(vertices[i].x, vertices[i].y, vertices[i].z);
		}
						
		return;
	}

	v12 = v1 + v2;
	v23 = v2 + v3;
	v31 = v3 + v1;
	v12.normalize();
	v23.normalize();
	v31.normalize();

	subdivide(v1,  v12, v31, depth-1);
	subdivide(v2,  v23, v12, depth-1);
	subdivide(v3,  v31, v23, depth-1);
	subdivide(v12, v23, v31, depth-1);
}

void Sphere::draw_ico_sphere(int num_of_divisions)
{	
	glBegin(GL_TRIANGLES);
	
	for(int i = 0; i < 24; i+=3)
	{
		subdivide(vertices[i+0],
			      vertices[i+1],
			      vertices[i+2],
			      num_of_divisions);
	}
	
	glEnd();
}

void Sphere::draw() const
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	set_transformation();
	set_material();
	
	glScaled(radius, radius, radius);
	glCallList(display_list);

	glPopMatrix();
}
