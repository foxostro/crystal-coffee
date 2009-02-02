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
    // TODO P1 draw the geometry in local space
}

