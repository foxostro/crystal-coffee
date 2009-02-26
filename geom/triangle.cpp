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
#include <cstring> // for memset

#include <iostream>
using namespace std;

Triangle::Triangle()
{
    memset(vertices, 0, sizeof vertices);
    memset(normals, 0, sizeof normals);
    memset(materials, 0, sizeof materials);
    memset(tcoords, 0, sizeof tcoords);
    memset(tangents, 0, sizeof tangents);
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
    
    CalculateTriangleTangent(vertices, tcoords, tangents);
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
    
    CalculateTriangleTangent(vertices, tcoords, tangents);
}

Triangle::~Triangle() { /* Do Nothing */ }

void Triangle::draw() const
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	set_transformation();
	set_material();

	glBegin(GL_TRIANGLES);

	for(int i=0; i<3; ++i)
	{
		if(effect && effect->areTangentsRequired())
		{
			glVertexAttrib3d(effect->getTangentAttribSlot(),
			                 tangents[i].x,
			                 tangents[i].y,
			                 tangents[i].z);
		}
		
		glTexCoord2d(tcoords[i].x, tcoords[i].y);
		glNormal3d(normals[i].x, normals[i].y, normals[i].z);
		glVertex3d(vertices[i].x, vertices[i].y, vertices[i].z);
	}

	glEnd();

	glPopMatrix();
}
