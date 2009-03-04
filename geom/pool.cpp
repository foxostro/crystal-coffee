/**
* @file pool.cpp
* @brief Generates pool geometry for the water pool scene
*
* @author Andrew Fox (arfox)
*/

#include "pool.h"

static void create_square(std::vector<Face> &faces,
						  const Vec3& xaxis,
						  const Vec3& yaxis,
						  const Vec3& corner,
						  const Vec3& normal,
						  const Vec2& tcoord_min,
						  const Vec2& tcoord_unit)
{
	Vec3 maxi = corner + xaxis + yaxis;

	Face face;

	face.normals[0] = normal;
	face.normals[1] = normal;
	face.normals[2] = normal;

#ifdef UV_CLOCKWISE
	Vec2 dist(xaxis.magnitude(), yaxis.magnitude());	
#else
	// computing uvs in counterclockwise order
	Vec2 dist(yaxis.magnitude(), xaxis.magnitude());
#endif

	/****************************************************************************/

	face.vertices[0] = maxi;
	face.vertices[1] = corner;
	face.vertices[2] = corner + yaxis;

	face.tcoords[0] = tcoord_min + dist * tcoord_unit;
	face.tcoords[1] = tcoord_min;
#ifdef UV_CLOCKWISE
	face.tcoords[2] = tcoord_min + Vec2(0, dist.y) * tcoord_unit;
#else
	face.tcoords[2] = tcoord_min + Vec2(dist.x, 0) * tcoord_unit;
#endif

	calculate_triangle_tangent(face.vertices, face.normals, face.tcoords, face.tangents);

	faces.push_back(face);

	/****************************************************************************/

	face.vertices[0] = corner;
	face.vertices[1] = maxi;
	face.vertices[2] = corner + xaxis;

	face.tcoords[0] = tcoord_min;
	face.tcoords[1] = tcoord_min + dist * tcoord_unit;
#ifdef UV_CLOCKWISE
	face.tcoords[2] = tcoord_min + Vec2(dist.x, 0) * tcoord_unit;
#else
	face.tcoords[2] = tcoord_min + Vec2(0, dist.y) * tcoord_unit;
#endif

	calculate_triangle_tangent(face.vertices, face.normals, face.tcoords, face.tangents);

	faces.push_back(face);

	/****************************************************************************/
}

TriangleSoup gen_pool_geometry(Scene * scene)
{
	assert(scene);

	std::vector<Face> faces;

	Vec2 tcmin(0,0);
	Vec2 tcunit(.25,.25);

	// so very, very disgusting. please don't look, your eyes may melt

	// upper corners
	create_square(faces,
		Vec3(POX-PIX,0,0), Vec3(0,0,2*POZ), Vec3(PIX,POY,-POZ),
		Vec3::UnitY, tcmin, tcunit);
	create_square(faces,
		Vec3(POX-PIX,0,0), Vec3(0,0,2*POZ), Vec3(-POX,POY,-POZ),
		Vec3::UnitY, tcmin, tcunit);

	create_square(faces,
		Vec3(2*PIX,0,0), Vec3(0,0,POZ-PIZ), Vec3(-PIX,POY,-POZ),
		Vec3::UnitY, tcmin, tcunit);
	create_square(faces,
		Vec3(2*PIX,0,0), Vec3(0,0,POZ-PIZ), Vec3(-PIX,POY,PIZ),
		Vec3::UnitY, tcmin, tcunit);

	// inner sides

	create_square(faces,
		Vec3(0,0,2*PIZ), Vec3(0,POY-PIY,0), Vec3(-PIX,PIY,-PIZ),
		Vec3::UnitX, tcmin, tcunit);

	create_square(faces,
		-Vec3(2*PIX,0,0), Vec3(0,POY-PIY,0), Vec3(PIX,PIY,-PIZ),
		Vec3::UnitZ, tcmin, tcunit);

	create_square(faces,
		-Vec3(0,POY-PIY,0), -Vec3(0,0,2*PIZ), Vec3(PIX,POY,PIZ),
		-Vec3::UnitX, tcmin, tcunit);

	create_square(faces,
		Vec3(2*PIX,0,0), Vec3(0,POY-PIY,0), Vec3(-PIX,PIY,PIZ),
		-Vec3::UnitZ, tcmin, tcunit);

	// outer sides
	create_square(faces,
		Vec3(0,POY-PBY,0), Vec3(0,0,2*POZ), Vec3(-POX,PBY,-POZ),
		-Vec3::UnitX, tcmin, tcunit);

	create_square(faces,
		Vec3(0,0,2*POZ), Vec3(0,POY-PBY,0), Vec3(POX,PBY,-POZ),
		Vec3::UnitX, tcmin, tcunit);

	create_square(faces,
		Vec3(2*POX,0,0), Vec3(0,POY-PBY,0), Vec3(-POX,PBY,-POZ),
		-Vec3::UnitZ, tcmin, tcunit);

	create_square(faces,
		Vec3(0,POY-PBY,0), Vec3(2*POX,0,0), Vec3(-POX,PBY,POZ),
		Vec3::UnitZ, tcmin, tcunit);

	// bottom
	create_square(faces,
		Vec3(0,0,2*PIZ), Vec3(2*PIX,0,0), Vec3(-PIX,PIY,-PIZ),
		Vec3::UnitY, tcmin, tcunit);
	create_square(faces,
		Vec3(0,0,2*POZ), Vec3(2*POX,0,0), Vec3(-POX,PBY,-POZ),
		-Vec3::UnitY, tcmin, tcunit);

	return TriangleSoup(scene, faces);
}
