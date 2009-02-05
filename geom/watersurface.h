/**
 * @file watersurface.h
 * @brief Class definition for WaterSurface.
 *
 * @author Kristin Siu (kasiu)
 * @author Eric Butler (edbutler)
 * @author Frank Palermo (fpalermo)
 */

/*
    EDIT THIS FILE FOR P1. However, do not change existing constructor
    signatures. The staff scene loader requires all of those to be intact.
 */

#ifndef _WATERSURFACE_H_
#define _WATERSURFACE_H_

#include "scene.h"
#include <vector>

class WaterSurface: public UpdatableGeometry
{
public:
	typedef struct { real_t x, y, z; } vertex_t;
	typedef unsigned int index_t;

    /**
     * structure containing information about a wave-emitting point.
     */
    struct WavePoint {
        Vec2 position; // position on surface (between (-1 and 1)
        real_t falloff; // exponential falloff of amplitude
        real_t coefficient; // scaling factor of amplitude
        real_t timerate; // scaling factor of time
        real_t period; // scaling factor of distance
    };

    typedef std::vector<WavePoint> WavePointList;

    /**
     * Construct a new watersurface.
     * @param pos The world position.
     * @param ori The world orientation.
     * @param scl The world scale.
     * @param wave_points The list of wave-emitting points.
     * @param resx The mesh resolution along the local x axis.
     * @param resz The mesh resolution along the local z azis.
     * @param mat The material to use for gl rendering.
     * @param efc The shader effect to use for gl rendering.
     */
    WaterSurface(const Vec3& pos, const Quat& ori, const Vec3& scl,
                 const WavePointList& wave_points,
                 int resx, int resz, Material* mat, Effect* efc=0);

    virtual ~WaterSurface();

    /**
     * Returns the absolute height of the watersurface (in the local
     * coordinate space) for the given (x,z) and time.
     * @param pos The x and z positions. Valid range is (-1,-1) to (1,1).
     * @param time The absolute time.
     * @return The value of y for x, z, and time in the local coordinate space.
     */
    real_t get_height(const Vec2& pos, real_t time);

    virtual void update(real_t time);

	virtual void draw() const;

private:
    // list of all wave-emitting points.
    WavePointList wave_points;

    // the resolution; the number of vertices in each direction minus one
    int resx, resz;

	vertex_t * vertices;
	vertex_t * normals;
	unsigned int * indices;
	unsigned int num_of_indices;
	unsigned int num_of_vertices;
	
	/** Generate indices for the heightmap geometry (these are set once) */
	void generate_indices();

	/** Gets a vertex from the vertices array given a position in the
	  * heightmap data.
	  * @param x X position in the rectangular heightmap
	  * @param z Z position in the rectangular heightmap
	  * @return reference to the vertex in question
	  */
	vertex_t & get_vertex(int x, int z);
	
	/** Gets a normal from the normals array given a position in the
	  * heightmap data.
	  * @param x X position in the rectangular heightmap
	  * @param z Z position in the rectangular heightmap
	  * @return reference to the normal in question
	  */
	vertex_t & get_normal(int x, int z);
	
	/** Gets a triangle normal given the heightmap-location of three vertices.
	  */
	Vec3 get_triangle_normal(int x1, int z1,
	                         int x2, int z2,
	                         int x3, int z3);
};

#endif /* _WATERSURFACE_H_ */

