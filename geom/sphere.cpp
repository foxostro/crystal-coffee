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

#include <iostream>
using namespace std;

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

Vec2 Sphere::texcoords[24] =
{
	Vec2(0.00, 1.00), // Top, East, 1
	Vec2(0.00, 0.50), // Top, East, 2
	Vec2(0.25, 0.50), // Top, East, 3
	
	Vec2(0.25, 1.00), // Top, North, 1
	Vec2(0.25, 0.50), // Top, North, 2
	Vec2(0.50, 0.50), // Top, North, 3
	
	Vec2(0.50, 1.00), // Top, West, 1
	Vec2(0.50, 0.50), // Top, West, 2
	Vec2(0.75, 0.50), // Top, West, 3
	
	Vec2(0.75, 1.00), // Top, South, 1
	Vec2(0.75, 0.50), // Top, South, 2
	Vec2(1.00, 0.50), // Top, South, 3





	Vec2(0.00, 0.00), // Bottom, East, 1
	Vec2(0.00, 0.50), // Bottom, East, 2
	Vec2(0.25, 0.50), // Bottom, East, 3
	
	Vec2(0.25, 0.00), // Bottom, North, 1
	Vec2(0.25, 0.50), // Bottom, North, 2
	Vec2(0.50, 0.50), // Bottom, North, 3
	
	Vec2(0.50, 0.00), // Bottom, West, 1
	Vec2(0.50, 0.50), // Bottom, West, 2
	Vec2(0.75, 0.50), // Bottom, West, 3
	
	Vec2(0.75, 0.00), // Bottom, South, 1
	Vec2(0.75, 0.50), // Bottom, South, 2
	Vec2(1.00, 0.50), // Bottom, South, 3
};

unsigned int Sphere::display_list = 0;

void Sphere::init_sphere()
{
	if(display_list == 0) {
		display_list = glGenLists(1);
		glNewList(display_list, GL_COMPILE);
		draw_ico_sphere(0);
		glEndList();
	}
}

Sphere::Sphere()
: radius(0)
{
	init_sphere();
}

Sphere::Sphere(const Vec3& pos, const Quat& ori, const Vec3& scl,
               real_t rad, Material* mat, Effect* efc)
: Geometry(pos, ori, scl, mat, efc),
  radius(rad)
{
	init_sphere();
}

Sphere::~Sphere()
{
	// do nothing
	// Display list is cleaned up when the OpenGL context is released.
}
   
void Sphere::subdivide(const Vec3 &v1,
                       const Vec3 &v2,
                       const Vec3 &v3,
                       const Vec2 &st1,
                       const Vec2 &st2,
                       const Vec2 &st3,
                       int depth)
{
	Vec3 v12, v23, v31;
	Vec2 st12, st23, st31;
	
	assert(depth>=0);

	if(depth == 0)
	{
		glTexCoord2d(st1.x, st1.y);
		glNormal3f((GLfloat)v1.x, (GLfloat)v1.y, (GLfloat)v1.z);
		glVertex3d(v1.x, v1.y, v1.z);
		
		glTexCoord2d(st2.x, st2.y);
		glNormal3f((GLfloat)v2.x, (GLfloat)v2.y, (GLfloat)v2.z);
		glVertex3d(v2.x, v2.y, v2.z);
		
		glTexCoord2d(st3.x, st3.y);
		glNormal3f((GLfloat)v3.x, (GLfloat)v3.y, (GLfloat)v3.z);
		glVertex3d(v3.x, v3.y, v3.z);
		
		return;
	}

	v12 = v1 + v2;
	v23 = v2 + v3;
	v31 = v3 + v1;
	v12.normalize();
	v23.normalize();
	v31.normalize();
	
	st12 = (st1 + st2) * 0.5;
	st23 = (st2 + st3) * 0.5;
	st31 = (st3 + st1) * 0.5;

	subdivide(v1,  v12, v31, st1,  st12, st31, depth-1);
	subdivide(v2,  v23, v12, st2,  st23, st12, depth-1);
	subdivide(v3,  v31, v23, st3,  st31, st23, depth-1);
	subdivide(v12, v23, v31, st12, st23, st31, depth-1);
}

void Sphere::draw_ico_sphere(int num_of_divisions)
{	
	glBegin(GL_TRIANGLES);
	
	for(int i = 0; i < 24; i+=3)
	{
		subdivide(vertices[i+0],
			      vertices[i+1],
			      vertices[i+2],
			      texcoords[i+0],
			      texcoords[i+1],
			      texcoords[i+2],
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
