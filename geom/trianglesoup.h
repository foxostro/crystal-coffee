/**
 * @file triangle.h
 * @brief Class definition for TriangleSoup.
 *
 * @author Andrew Fox (arfox)
 */

#ifndef _TRIANGLE_SOUP_H_
#define _TRIANGLE_SOUP_H_

#include "scene.h"

/** An unindexed collection of triangles stored in BufferObjects. */
class TriangleSoup
{
public:
	~TriangleSoup(void);
	TriangleSoup(void);
	TriangleSoup(Scene * scene, const std::vector<Face> &faces);
	void create(Scene * scene, const std::vector<Face> &faces);

public:
	boost::shared_ptr< BufferObject<Vec4> > tangents_buffer;
	boost::shared_ptr< BufferObject<Vec3> > normals_buffer;
	boost::shared_ptr< BufferObject<Vec3> > vertices_buffer;
	boost::shared_ptr< BufferObject<Vec2> > tcoords_buffer;
};

#endif

