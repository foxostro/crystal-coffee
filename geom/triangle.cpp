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
    
    calculate_tangents();
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
    
    calculate_tangents();
}

/*
Lengyel, Eric. “Computing Tangent Space Basis Vectors for an Arbitrary Mesh”. Terathon Software 3D Graphics Library, 2001. http://www.terathon.com/code/tangent.html
*/
void Triangle::calculate_tangents()
{
	Vec3 tan1[6];
    Vec3 * tan2 = tan1 + 3;
    memset(tan1, 0, 3 * sizeof(Vec3) * 2);
            
	const Vec3 &v1 = vertices[0];
	const Vec3 &v2 = vertices[1];
	const Vec3 &v3 = vertices[2];

	const Vec2 &w1 = tcoords[0];
	const Vec2 &w2 = tcoords[1];
	const Vec2 &w3 = tcoords[2];

	real_t x1 = v2.x - v1.x;
	real_t x2 = v3.x - v1.x;
	real_t y1 = v2.y - v1.y;
	real_t y2 = v3.y - v1.y;
	real_t z1 = v2.z - v1.z;
	real_t z2 = v3.z - v1.z;

	real_t s1 = w2.x - w1.x;
	real_t s2 = w3.x - w1.x;
	real_t t1 = w2.y - w1.y;
	real_t t2 = w3.y - w1.y;

	real_t r = 1.0 / (s1 * t2 - s2 * t1);
	
	Vec3 sdir((t2 * x1 - t1 * x2) * r,
	          (t2 * y1 - t1 * y2) * r,
	          (t2 * z1 - t1 * z2) * r);
	              
	Vec3 tdir((s1 * x2 - s2 * x1) * r,
	          (s1 * y2 - s2 * y1) * r,
	          (s1 * z2 - s2 * z1) * r);

	tan1[0] = sdir;
	tan1[1] = sdir;
	tan1[2] = sdir;

	tan2[0] = tdir;
	tan2[1] = tdir;
	tan2[2] = tdir;
    
    for (long a = 0; a < 3; a++)
    {
        const Vec3 &n = normals[a];
        const Vec3 &t = tan1[a];
        
        // Gram-Schmidt orthogonalize
        tangents[a] = (t - n * n.dot(t)).normalize();
    }
}

Triangle::~Triangle() {}

void Triangle::draw() const
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	set_transformation();
	set_material();

	glBegin(GL_TRIANGLES);

	for(int i=0; i<3; ++i)
	{
		assert(abs(tangents[i].dot(normals[i])) < 0.001);
		
		glTexCoord2d(tcoords[i].x, tcoords[i].y);
		glNormal3d(normals[i].x, normals[i].y, normals[i].z);
		glVertexAttrib3fARB(1, tangents[i].x, tangents[i].y, tangents[i].z);
		glVertex3d(vertices[i].x, vertices[i].y, vertices[i].z);
	}

	glEnd();

	glPopMatrix();
}
