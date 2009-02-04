/**
 * @file triangle.cpp
 * @brief Function definitions for the Triangle class.
 *
 * @author Eric Butler (edbutler)
 */

/*
    EDIT THIS FILE FOR P1. However, do not change existing constructor
    signatures. The staff scene loader requires all of those to be intact.
 */

#include "vec/mat.h"
#include "triangle.h"
#include "glheaders.h"
#include <cstring>

Triangle::Triangle()
{
    memset(vertices, 0, sizeof vertices);
    memset(normals, 0, sizeof normals);
    memset(materials, 0, sizeof materials);
    memset(tcoords, 0, sizeof tcoords);
}

Triangle::Triangle(const Vec3& pos, const Quat& ori, const Vec3& scl,
                   const Vec3 vertices[3], const Vec2 tcoords[3],
                   const Vec3 normals[3], Material* materials[3], Effect* efc)
    : Geometry(pos, ori, scl, materials[0], efc)
{
    memcpy(this->vertices, vertices, sizeof this->vertices);
    memcpy(this->normals, normals, sizeof this->normals);
    memcpy(this->materials, materials, sizeof this->materials);
    memcpy(this->tcoords, tcoords, sizeof this->tcoords);
}

Triangle::Triangle(const Vec3& pos, const Quat& ori, const Vec3& scl,
                   const Vec3 vertices[3], const Vec2 tcoords[3],
                   const Vec3& normal, Material* mat, Effect* efc)
    : Geometry(pos, ori, scl, mat, efc)
{
    memcpy(this->vertices, vertices, sizeof this->vertices);
    memcpy(this->tcoords, tcoords, sizeof this->tcoords);
    normals[0] = normals[1] = normals[2] = normal;
    materials[0] = materials[1] = materials[2] = mat;
}

Triangle::~Triangle() {}

void Triangle::draw() const
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	set_transformation();
	set_material();

	glBegin(GL_TRIANGLES);

	glTexCoord2f(tcoords[2].x, tcoords[2].y);
	glVertex3f(vertices[2].x, vertices[2].y, vertices[2].z);
	glNormal3f(normals[2].x, normals[2].y, normals[2].z);

	glTexCoord2f(tcoords[1].x, tcoords[1].y);
	glVertex3f(vertices[1].x, vertices[1].y, vertices[1].z);
	glNormal3f(normals[1].x, normals[1].y, normals[1].z);

	glTexCoord2f(tcoords[0].x, tcoords[0].y);
	glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
	glNormal3f(normals[0].x, normals[0].y, normals[0].z);

	glEnd();

	glPopMatrix();
}
