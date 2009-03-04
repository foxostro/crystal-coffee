/**
 * @file watersurface.h
 * @brief Class definition for WaterSurface.
 *
 * @author Kristin Siu (kasiu)
 * @author Eric Butler (edbutler)
 * @author Frank Palermo (fpalermo)
 * @author Andrew Fox (arfox)
 */

#ifndef _WATERSURFACE_H_
#define _WATERSURFACE_H_

#include "scene.h"
#include <vector>

class WaterSurface
{
public:
    /**
     * Structure containing information about a wave-emitting point.
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
     * @param wave_points The list of wave-emitting points.
     * @param resx The mesh resolution along the local x axis.
     * @param resz The mesh resolution along the local z azis.
     */
    WaterSurface(Scene * scene,
	             const WavePointList& wave_points,
				 int resx, int resz);

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

	BufferObject<Vec3> * vertices_buffer;
	BufferObject<Vec3> * normals_buffer;
	BufferObject<index_t> * indices_buffer;

private:
    // list of all wave-emitting points.
    WavePointList wave_points;

    // the resolution; the number of vertices in each direction minus one
    int resx, resz;

	real_t * heightmap;
	
	// generate indices for the heightmap mesh
	void generate_indices();
	
	// generate the heightmap from the surface function
	void generate_heightmap(real_t time);
	
	// generate the normals of the heightmap
	void generate_normals();
	
	// generate vertices for the mesh from the heightmap
	void generate_vertices();
	
	void set_vertex(Vec3 * vertices, int x, int z, Vec3 v);
	
	void set_normal(Vec3 * normals, int x, int z, Vec3 n);
	
	static Vec3 compute_normal(real_t *heightmap, int sx, int sz, int x, int z);
};

#endif /* _WATERSURFACE_H_ */

