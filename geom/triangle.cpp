/**
 * @file triangle.cpp
 * @brief Function definitions for the Triangle class.
 *
 * @author Eric Butler (edbutler)
 */

#include "vec/mat.h"
#include "triangle.h"
#include "glheaders.h"
#include <cstring> // for memset

#include <iostream>
using namespace std;

Triangle::Triangle(const Vec3 vertices[3],
                   const Vec2 tcoords[3],
                   const Vec3 normals[3])
{
	vertices_buffer.create(3, vertices, STATIC_DRAW);
	normals_buffer.create(3, normals, STATIC_DRAW);
	tcoords_buffer.create(3, tcoords, STATIC_DRAW);
	
	// Generate triangle tangents
	Vec4 tangents[3];
    calculate_triangle_tangent(vertices, normals, tcoords, tangents);
	tangents_buffer.create(3, tangents, STATIC_DRAW);
}

