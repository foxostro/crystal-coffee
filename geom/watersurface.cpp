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
                           int resx, int resz)
: UpdatableGeometry(pos, ori, scl),
  wave_points(wave_points), resx(resx), resz(resz),
  heightmap(0)
{
	const size_t num_of_vertices = (resx+1) * (resz+1);

	// Create the heightmap buffer and clear it
	heightmap = new real_t[num_of_vertices];
	memset(heightmap, 0, sizeof(real_t) * num_of_vertices);

	// Create the vertex buffer, and clear it
	{
		Vec3 * vertices = new Vec3[num_of_vertices];
		memset(vertices, 0, sizeof(Vec3) * num_of_vertices);
		vertex_buffer.recreate(num_of_vertices, vertices, DYNAMIC_DRAW);
		delete [] vertices;
	}
	
	// Create the normals buffer, and clear it
	{
		Vec3 * normals = new Vec3[num_of_vertices];
		memset(normals, 0, sizeof(Vec3) * num_of_vertices);
		normals_buffer.recreate(num_of_vertices, normals, DYNAMIC_DRAW);
		delete [] normals;
	}

	// Create the normals buffer, and clear it
	{
		Vec3 * normals = new Vec3[num_of_vertices];
		memset(normals, 0, sizeof(Vec3) * num_of_vertices);
		normals_buffer.recreate(num_of_vertices, normals, DYNAMIC_DRAW);
		delete [] normals;
	}

	// Create the indices buffer and define the mesh topology
	{
		const size_t num_of_indices = resx * resz * 6;
		index_t * indices = new index_t[num_of_indices];

		size_t idx;
		int x, z;

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
				assert(idx <= num_of_indices);
			}
		}

		index_buffer.recreate(num_of_indices, indices, STATIC_DRAW);
		delete [] indices;
	}

	update(0.0);
}

WaterSurface::~WaterSurface()
{
	delete [] heightmap;
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

Vec3 WaterSurface::compute_normal(real_t *heightmap,
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
	
	Vec3 n;
	n.x = nv.x;
	n.y = nv.y;
	n.z = nv.z;
	return n;
}

void WaterSurface::generate_normals()
{
	Vec3 * normals = (Vec3 *)normals_buffer.lock();

	for(int x=0; x<=resx; x++)
	{
		for(int z=0; z<=resz; z++)
		{
			set_normal(normals, x, z,
			           compute_normal(heightmap, resx, resz, x, z));
		}
	}

	normals_buffer.unlock();
}

void WaterSurface::generate_vertices()
{
#define NX(x) ((real_t)(x)/resx*2-1)
#define NZ(z) ((real_t)(z)/resz*2-1)

	Vec3 * vertices = (Vec3 *)vertex_buffer.lock();

	// fill in vertices
	for(int x=0; x <=resx; x++)
	{
		for(int z=0; z<=resz; z++)
		{
			// get the vertex height
			Vec3 v;
			v.x = NX(x);
			v.z = NZ(z);
			v.y = heightmap[x*resz + z];
			set_vertex(vertices, x, z, v);
		}
	}

	vertex_buffer.unlock();
	
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

	// Bind the normals buffer
	glEnableClientState(GL_NORMAL_ARRAY);
	normals_buffer.bind();
	glNormalPointer(GL_DOUBLE, 0, 0);

	// Bind the vertex buffer
	glEnableClientState(GL_VERTEX_ARRAY);
	vertex_buffer.bind();
	glVertexPointer(3, GL_DOUBLE, 0, 0);

	// And actually render the batch using the element buffer object's indices
	GLsizei count = index_buffer.getNumber();
	index_buffer.bind();
	glDrawElements(GL_TRIANGLES, count, MESH_INDEX_FORMAT, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glPopMatrix();
}

void WaterSurface::set_vertex(Vec3 * vertices, int x, int z, Vec3 v)
{
	assert(x <= resx);
	assert(x >= 0);
	assert(z <= resz);
	assert(z >= 0);
	vertices[x*(resz+1)+z] = v;
}

void WaterSurface::set_normal(Vec3 * normals, int x, int z, Vec3 n)
{
	assert(x <= resx);
	assert(x >= 0);
	assert(z <= resz);
	assert(z >= 0);
	normals[x*(resz+1)+z] = n;
}

