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

static const real_t X = 0.525731112119133606;
static const real_t Z = 0.850650808352039932;   
	
real_t Sphere::vdata[12][3] =
{
   {-X, 0.0,  Z},
   { X, 0.0,  Z},
   {-X, 0.0, -Z}, 
   { X, 0.0, -Z},
   
   {0.0,  Z,  X},
   {0.0,  Z, -X},
   {0.0, -Z,  X},
   {0.0, -Z, -X},
   
   { Z,  X, 0.0},
   {-Z,  X, 0.0},
   { Z, -X, 0.0},
   {-Z, -X, 0.0}
};

int Sphere::tindices[20][3] =
{
   {0,4,1},
   {0,9,4},
   {9,5,4},
   {4,5,8},
   {4,8,1},
   
   {8,10,1},
   {8,3,10},
   {5,3,8},
   {5,2,3},
   {2,7,3},
   
   {7,10,3},
   {7,6,10},
   {7,11,6},
   {11,0,6},
   {0,1,6},
   
   {6,1,10},
   {9,0,11},
   {9,11,2},
   {9,2,5},
   {7,2,11}
};

unsigned int Sphere::display_list = 0;

void Sphere::init_sphere()
{
	if(display_list == 0) {
		display_list = glGenLists(1);
		glNewList(display_list, GL_COMPILE);
		draw_ico_sphere(5);
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
   
void Sphere::subdivide(Vec3 v1, Vec3 v2, Vec3 v3, int depth)
{
	Vec3 v12, v23, v31;
	
	assert(depth>=0);

	if(depth == 0)
	{
		glNormal3f((GLfloat)v1.x, (GLfloat)v1.y, (GLfloat)v1.z);
		glVertex3d(v1.x, v1.y, v1.z);
		
		glNormal3f((GLfloat)v2.x, (GLfloat)v2.y, (GLfloat)v2.z);
		glVertex3d(v2.x, v2.y, v2.z);
		
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

	subdivide(v1, v12, v31, depth-1);
	subdivide(v2, v23, v12, depth-1);
	subdivide(v3, v31, v23, depth-1);
	subdivide(v12, v23, v31, depth-1);
}

void Sphere::draw_ico_sphere(int num_of_divisions)
{	
	glBegin(GL_TRIANGLES);
	
	for(int i = 0; i < 20; ++i)
	{
		subdivide(Vec3(vdata[tindices[i][0]]),
			      Vec3(vdata[tindices[i][1]]),
			      Vec3(vdata[tindices[i][2]]),
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
