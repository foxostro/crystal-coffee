/**
 * @file triangle.cpp
 * @brief Function definitions for the TriangleSoup class.
 *
 * @author Andrew Fox (arfox)
 */

#include "vec/mat.h"
#include "trianglesoup.h"
#include "glheaders.h"
#include <memory.h>

TriangleSoup::~TriangleSoup() { /* Do Nothing */ }

TriangleSoup::TriangleSoup() { /* Do Nothing */ }

TriangleSoup::TriangleSoup(Scene * scene, const std::vector<Face> &faces)
{
	create(scene, faces);
}

void TriangleSoup::create(Scene * scene, const std::vector<Face> &faces)
{
	assert(scene);

	size_t num_of_vertices = faces.size()*3;

	vertices_buffer = boost::shared_ptr<BufferObject<Vec3> >(new BufferObject<Vec3>());
	normals_buffer  = boost::shared_ptr<BufferObject<Vec3> >(new BufferObject<Vec3>());
	tangents_buffer = boost::shared_ptr<BufferObject<Vec4> >(new BufferObject<Vec4>());
	tcoords_buffer  = boost::shared_ptr<BufferObject<Vec2> >(new BufferObject<Vec2>());

	// Create buffer objects from the vector of faces
	vertices_buffer->create(num_of_vertices, STATIC_DRAW);
	normals_buffer->create(num_of_vertices, STATIC_DRAW);
	tangents_buffer->create(num_of_vertices, STATIC_DRAW);
	tcoords_buffer->create(num_of_vertices, STATIC_DRAW);

	Vec3 * vertices = vertices_buffer->lock();
	Vec3 * normals = normals_buffer->lock();
	Vec4 * tangents = tangents_buffer->lock();
	Vec2 * tcoords = tcoords_buffer->lock();

	for(std::vector<Face>::const_iterator i=faces.begin();
		i != faces.end(); ++i)
	{
		memcpy(vertices, (*i).vertices, sizeof(Vec3)*3);
		memcpy(normals, (*i).normals, sizeof(Vec3)*3);
		memcpy(tangents, (*i).tangents, sizeof(Vec4)*3);
		memcpy(tcoords, (*i).tcoords, sizeof(Vec2)*3);

		vertices += 3;
		normals += 3;
		tangents += 3;
		tcoords += 3;
	}

	vertices_buffer->unlock();
	normals_buffer->unlock();
	tangents_buffer->unlock();
	tcoords_buffer->unlock();
}
