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
  heightmap(0),
  vertices(0),
  normals(0),
  indices(0),
  num_of_indices(0),
  num_of_vertices(0)
{
	num_of_vertices = (resx+1) * (resz+1);
	heightmap = new real_t[num_of_vertices];
	vertices = new vertex_t[num_of_vertices];
	normals = new vertex_t[num_of_vertices];

	num_of_indices = resx * resz * 6;
	indices = new index_t[num_of_indices];

	memset(heightmap, 0, sizeof(real_t) * num_of_vertices);
	memset(vertices, 0, sizeof(vertex_t) * num_of_vertices);
	memset(normals, 0, sizeof(vertex_t) * num_of_vertices);
	memset(indices, 0, sizeof(index_t) * num_of_indices);

	generate_indices();

	update(0.0);
}

WaterSurface::~WaterSurface()
{
	delete [] heightmap;
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

void WaterSurface::generate_indices()
{
	int idx, x, z;
	
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
}

void WaterSurface::generate_heightmap(real_t time)
{
#define NX(x) ((real_t)(x)/resx*2-1)
#define NZ(z) ((real_t)(z)/resz*2-1)

	for(int x=0; x<=resx; x++)
	{
		for(int z=0; z<=resz; z++)
		{
			heightmap[x*resz + z] = get_height(Vec2(NX(x), NZ(z)), time);
		}
	}
	
#undef NX
#undef NZ
}

WaterSurface::vertex_t
WaterSurface::compute_normal(real_t *heightmap,
                             int sx, int sz,
                             int x, int z)
{
	assert(heightmap);
	
	const float y11 = heightmap[(x+0)*sz + (z+0)];
	const float y20 = (x + 1 < sx) ? (heightmap[(x+1)*sz + (z+0)]) : y11;
	const float y02 = (z + 1 < sz) ? (heightmap[(x+0)*sz + (z+1)]) : y11;
	const float y01 = (x - 1 >= 0) ? (heightmap[(x-1)*sz + (z+0)]) : y11;
	const float y10 = (z - 1 >= 0) ? (heightmap[(x+0)*sz + (z-1)]) : y11;
	
	Vec3 nv;
	nv.x = y01 - y20;
	nv.z = y10 - y02;
	nv.y = 8.0 / sx; // 0.03
	nv.normalize();
	
	vertex_t n;
	n.x = nv.x;
	n.y = nv.y;
	n.z = nv.z;
	return n;
}

void WaterSurface::generate_normals()
{	
	for(int x=0; x<=resx; x++)
	{
		for(int z=0; z<=resz; z++)
		{
			set_normal(x, z, compute_normal(heightmap, resx, resz, x, z));
		}
	}
}

void WaterSurface::generate_vertices()
{
#define NX(x) ((real_t)(x)/resx*2-1)
#define NZ(z) ((real_t)(z)/resz*2-1)

	// fill in vertices
	for(int x=0; x <=resx; x++)
	{
		for(int z=0; z<=resz; z++)
		{
			// get the vertex height
			vertex_t v;
			v.x = NX(x);
			v.z = NZ(z);
			v.y = heightmap[x*resz + z];
			set_vertex(x, z, v);
		}
	}
	
#undef NX
#undef NZ
}

void WaterSurface::update(real_t time)
{
	generate_heightmap(time);
	generate_normals();
	generate_vertices();
}

void WaterSurface::draw() const
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	set_transformation();
	set_material();

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_DOUBLE, 0, normals);
	glVertexPointer(3, GL_DOUBLE, 0, vertices);

	glDrawElements(GL_TRIANGLES,
	               num_of_indices,
				   GL_UNSIGNED_INT,
				   indices);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glPopMatrix();
}

void WaterSurface::set_vertex(int x, int z, vertex_t v)
{
	assert(x <= resx);
	assert(x >= 0);
	assert(z <= resz);
	assert(z >= 0);
	vertices[x*(resz+1)+z] = v;
}

void WaterSurface::set_normal(int x, int z, vertex_t n)
{
	assert(x <= resx);
	assert(x >= 0);
	assert(z <= resz);
	assert(z >= 0);
	normals[x*(resz+1)+z] = n;
}

