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

#include "watersurface.h"
#include "glheaders.h"
#include <iostream>

WaterSurface::WaterSurface(const Vec3& pos, const Quat& ori, const Vec3& scl,
                           const WavePointList& wave_points,
                           int resx, int resz, Material* mat, Effect* efc)
    : UpdatableGeometry(pos, ori, scl, mat, efc),
      wave_points(wave_points), resx(resx), resz(resz) {}

WaterSurface::~WaterSurface() {}

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
}

void WaterSurface::draw() const
{
    // TODO P1 draw the geometry in local space
}

