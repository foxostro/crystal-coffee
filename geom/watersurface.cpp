/**
 * @file watersurface.cpp
 * @brief Function definitions for the WaterSurface class.
 *
 * @author Kristin Siu (kasiu)
 * @author Eric Butler (edbutler)
 * @author Frank Palermo (fpalermo)
 * @author Andrew Fox (arfox)
 */

#include "vec/mat.h"
#include "watersurface.h"
#include "glheaders.h"
#include <iostream>
#include <cstring>

WaterSurface::WaterSurface(Scene * scene,
						   const WavePointList& wave_points,
                           int resx, int resz)
: wave_points(wave_points), resx(resx), resz(resz),
  heightmap(0),
  vertices_buffer(0),
  normals_buffer(0),
  tcoords_buffer(0),
  indices_buffer(0)
{
	assert(scene);

	const size_t num_of_vertices = (resx+1) * (resz+1);

	// Create the heightmap buffer and clear it
	heightmap = new real_t[num_of_vertices];
	memset(heightmap, 0, sizeof(real_t) * num_of_vertices);

	// Create the vertices buffer
	vertices_buffer = new BufferObject<Vec3>();
	vertices_buffer->create(num_of_vertices, DYNAMIC_DRAW);
	scene->resources.push_back(vertices_buffer);

	// Create the normals buffer
	normals_buffer = new BufferObject<Vec3>();
	normals_buffer->create(num_of_vertices, DYNAMIC_DRAW);
	scene->resources.push_back(normals_buffer);

	// Create the tcoords buffer
	tcoords_buffer = new BufferObject<Vec2>();
	tcoords_buffer->create(num_of_vertices, STATIC_DRAW);
	scene->resources.push_back(tcoords_buffer);
	generate_tcoords();
	
	// Create the indices buffer to define the mesh topology
	generate_indices(scene);

	tick(0.0);
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
	
	const real_t y11 = heightmap[(x+0)*sz + (z+0)];
	const real_t y20 = (x + 1 < sx) ? (heightmap[(x+1)*sz + (z+0)]) : y11;
	const real_t y02 = (z + 1 < sz) ? (heightmap[(x+0)*sz + (z+1)]) : y11;
	const real_t y01 = (x - 1 >= 0) ? (heightmap[(x-1)*sz + (z+0)]) : y11;
	const real_t y10 = (z - 1 >= 0) ? (heightmap[(x+0)*sz + (z-1)]) : y11;
	
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
	assert(normals_buffer);

	Vec3 * normals = normals_buffer->lock();

	for(int x=0; x<=resx; x++)
	{
		for(int z=0; z<=resz; z++)
		{
			set_normal(normals, x, z,
			           compute_normal(heightmap, resx, resz, x, z));
		}
	}

	normals_buffer->unlock();
}

void WaterSurface::generate_tcoords()
{
	assert(tcoords_buffer);

	Vec2 * tcoords = tcoords_buffer->lock();

	for(int x=0; x<=resx; x++)
	{
		for(int z=0; z<=resz; z++)
		{
			set_tcoord(tcoords, x, z, Vec2((real_t)x/resx, (real_t)z/resz));
		}
	}

	tcoords_buffer->unlock();
}

void WaterSurface::generate_vertices()
{
#define NX(x) ((real_t)(x)/resx*2-1)
#define NZ(z) ((real_t)(z)/resz*2-1)

	assert(vertices_buffer);

	Vec3 * vertices = vertices_buffer->lock();

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

	vertices_buffer->unlock();
	
#undef NX
#undef NZ
}

void WaterSurface::tick(real_t time)
{
	generate_heightmap(time);
	generate_normals();
	generate_vertices();
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

void WaterSurface::set_tcoord(Vec2 * tcoords, int x, int z, Vec2 st)
{
	assert(x <= resx);
	assert(x >= 0);
	assert(z <= resz);
	assert(z >= 0);
	tcoords[x*(resz+1)+z] = st;
}

void WaterSurface::generate_indices( Scene * scene )
{
	assert(scene);

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

	// Create the indices buffer
	indices_buffer = new BufferObject<index_t>();
	indices_buffer->recreate(num_of_indices, indices, STATIC_DRAW);
	scene->resources.push_back(indices_buffer);

	delete [] indices;
}