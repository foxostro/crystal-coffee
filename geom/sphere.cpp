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

Sphere::Sphere()
: radius(0)
{
	// do nothing
}

Sphere::Sphere(const Vec3& pos, const Quat& ori, const Vec3& scl,
               real_t rad, Material* mat, Effect* efc)
: Geometry(pos, ori, scl, mat, efc),
  radius(rad)
{
	// do nothing
}

Sphere::~Sphere()
{
	// do nothing
}

void Sphere::draw() const
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	set_transformation();
	set_material();

	glutSolidSphere(radius, 20, 16);

	glPopMatrix();
}
