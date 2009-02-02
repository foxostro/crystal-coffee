/**
 * @file vec.cpp
 * @brief Vector classes.
 *
 * @author Eric Butler (edbutler)
 */

#include "vec.h"

const Vec2 Vec2::Zero = Vec2(0,0);
const Vec2 Vec2::Ones = Vec2(1,1);
const Vec2 Vec2::UnitX = Vec2(1,0);
const Vec2 Vec2::UnitY = Vec2(0,1);

std::ostream& operator<<(std::ostream& os, const Vec2& v)
{
    return os << '(' << v.x << ',' << v.y << ')';
}

std::istream& operator>>(std::istream& is, Vec2& v)
{
    char c;
    real_t x,y;
    is >> c >> x >> c >> y >> c;
    if (!is.fail()){
        v.x=x;
        v.y=y;
    }
    return is;
}

const Vec3 Vec3::Zero = Vec3(0,0,0);
const Vec3 Vec3::Ones = Vec3(1,1,1);
const Vec3 Vec3::UnitX = Vec3(1,0,0);
const Vec3 Vec3::UnitY = Vec3(0,1,0);
const Vec3 Vec3::UnitZ = Vec3(0,0,1);

std::ostream& operator<<(std::ostream& os, const Vec3& v)
{
    return os << '(' << v.x << ',' << v.y << ',' << v.z << ')';
}

std::istream& operator>>(std::istream& is, Vec3& v)
{
    char c;
    real_t x,y,z;
    is >> c >> x >> c >> y >> c >> z >> c;
    if (!is.fail()){
        v.x=x;
        v.y=y;
        v.z=z;
    }
    return is;
}

const Vec4 Vec4::Zero = Vec4(0,0,0,0);
const Vec4 Vec4::Ones = Vec4(1,1,1,1);
const Vec4 Vec4::UnitX = Vec4(1,0,0,0);
const Vec4 Vec4::UnitY = Vec4(0,1,0,0);
const Vec4 Vec4::UnitZ = Vec4(0,0,1,0);
const Vec4 Vec4::UnitW = Vec4(0,0,0,1);

std::ostream& operator<<(std::ostream& os, const Vec4& v)
{
    return os << '(' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << ')';
}

std::istream& operator>>(std::istream& is, Vec4& v)
{
    char c;
    real_t x,y,z,w;
    is >> c >> x >> c >> y >> c >> z >> c >> w >> c;
    if (!is.fail()){
        v.x=x;
        v.y=y;
        v.z=z;
        v.w=w;
    }
    return is;
}

