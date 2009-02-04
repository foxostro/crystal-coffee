/**
 * @file watersurface.cpp
 * @brief Function definitions for the WaterSurface class.
 *
 * @author Kristin Siu (kasiu)
 * @author Eric Butler (edbutler)
 * @author Frank Palermo (fpalermo)
 */

/*
    EDIT THIS FILE FOR P1. However, do not change existing constructor
    signatures. The staff scene loader requires all of those to be intact.
 */

#include "vec/mat.h"
#include "watersurface.h"
#include "glheaders.h"
#include <iostream>
#include <cstring>

WaterSurface::WaterSurface(const Vec3& pos, const Quat& ori, const Vec3& scl,
                           const WavePointList& wave_points,
                           int resx, int resz, Material* mat, Effect* efc)
: UpdatableGeometry(pos, ori, scl, mat, efc),
  wave_points(wave_points), resx(resx), resz(resz),
  num_of_indices(0),
  vertices(0),
  indices(0)
{
	int idx, x, z;

	num_of_vertices = (resx+1) * (resz+1);
	vertices = new vertex_t[num_of_vertices];
	normals = new vertex_t[num_of_vertices];

	num_of_indices = resx * resz * 6;
	indices = new index_t[num_of_indices];

	memset(vertices, 0, sizeof(vertex_t) * num_of_vertices);
	memset(normals, 0, sizeof(vertex_t) * num_of_vertices);
	memset(indices, 0, sizeof(index_t) * num_of_indices);

	for(idx = 0, x = 0; x < resx; x++)
	{
		for(z = 0; z < resz; z++)
		{
			// triangle 1
			indices[idx+0] = (x+0)*(resz+1) + (z+1);
			indices[idx+1] = (x+1)*(resz+1) + (z+0);
			indices[idx+2] = (x+0)*(resz+1) + (z+0);

			// triangle 2
			indices[idx+3] = (x+0)*(resz+1) + (z+1);
			indices[idx+4] = (x+1)*(resz+1) + (z+1);
			indices[idx+5] = (x+1)*(resz+1) + (z+0);

			idx+=6;
			assert((unsigned)idx <= num_of_indices);
		}
	}

	update(0.0);
}

WaterSurface::~WaterSurface()
{
	delete [] vertices;
	delete [] normals;
	delete [] indices;
}

real_t WaterSurface::get_height(const Vec2& pos, real_t time)
{
    real_t h = 0;

    for (WavePointList::iterator i=wave_points.begin();
            i != wave_points.end(); ++i) {
        WavePoint& p = *i;
        real_t r = pos.distance(p.position);
        h += p.coefficient * exp(-p.falloff * r ) *
            sin(p.period * r + p.timerate * time);
    }

    return h;
}

void WaterSurface::update(real_t time)
{
    // TODO P1 create a model for the given time using get_height
    // valid values for get_height range between (-1,-1) and (1,1), i.e.
    // the model's width and depth should be the unit square, -1 to 1.
    // the height at (x,y,z) is given by 'y = get_height(Vec2(x,z), time)'

	// update all vertex positions
	for(int x = 0; x < (resx+1); x++)
	{
		for(int z = 0; z < (resz+1); z++)
		{
			get_vertex(x,z).x = (real_t)x / resx * 2 - 1;
			get_vertex(x,z).z = (real_t)z / resz * 2 - 1;
			get_vertex(x,z).y = get_height(Vec2(get_vertex(x,z).x, get_vertex(x,z).z), time);

			get_normal(x,z).x = 0.0;
			get_normal(x,z).z = 0.0;
			get_normal(x,z).y = 1.0;
		}
	}

	// for each vertex, calculate normals of each face around it and average
	for(int x = 0; x <= resx; x++)
	{
		for(int z = 0; z <= resz; z++)
		{
			Vec3 normal1 = get_triangle_normal(x+0, z+0,  x-1, z+0,  x-1, z+1);
			Vec3 normal2 = get_triangle_normal(x+0, z+0,  x-1, z+1,  x+0, z+1);
			Vec3 normal3 = get_triangle_normal(x+0, z+0,  x+0, z+1,  x+1, z+0);
			Vec3 normal4 = get_triangle_normal(x+0, z+0,  x+1, z+0,  x+1, z-1);
			Vec3 normal5 = get_triangle_normal(x+0, z+0,  x+1, z-1,  x+0, z-1);
			Vec3 normal6 = get_triangle_normal(x+0, z+0,  x+0, z-1,  x-1, z+0);

			Vec3 normal = (normal1 + normal2 + normal3 + normal4 + normal5 + normal6) * (1.0 / 6.0);

			get_normal(x,z).x = normal.x;
			get_normal(x,z).y = normal.y;
			get_normal(x,z).z = normal.z;
		}
	}
}

void WaterSurface::draw() const
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	set_transformation();
	set_material();

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_DOUBLE, 0, vertices);
	glNormalPointer(GL_DOUBLE, 0, normals);

	glDrawElements(GL_TRIANGLES,
	               num_of_indices,
				   GL_UNSIGNED_INT,
				   indices);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glPopMatrix();
}

WaterSurface::vertex_t & WaterSurface::get_vertex( int x, int z )
{
	assert(x <= resx);
	assert(x >= 0);
	assert(z <= resz);
	assert(z >= 0);

	return vertices[x*(resz+1)+z];
}

WaterSurface::vertex_t & WaterSurface::get_normal( int x, int z )
{
	assert(x <= resx);
	assert(x >= 0);
	assert(z <= resz);
	assert(z >= 0);

	return normals[x*(resz+1)+z];
}

Vec3 WaterSurface::get_triangle_normal(int x1, int z1,
						               int x2, int z2,
						               int x3, int z3)
{
	Vec3 a, b, c, n;

	if(x1<=0 || x1>resx ||
	   x2<=0 || x2>resx ||
	   x3<=0 || x3>resx ||
	   z1<=0 || z1>resx ||
	   z2<=0 || z2>resx ||
	   z3<=0 || z3>resx)
	{
		return Vec3(0, 0, 1);
	}

	vertex_t &vertices_a = get_vertex(x1, z1);
	vertex_t &vertices_b = get_vertex(x2, z2);
	vertex_t &vertices_c = get_vertex(x3, z3);

	a.x = vertices_a.x;
	a.y = vertices_a.y;
	a.z = vertices_a.z;

	b.x = vertices_b.x;
	b.y = vertices_b.y;
	b.z = vertices_b.z;

	c.x = vertices_c.x;
	c.y = vertices_c.y;
	c.z = vertices_c.z;

	n = Vec3(c-a).cross(c-b);
	n.normalize();
	return n;
}
