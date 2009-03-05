/**
 * @file vec.h
 * @brief Vector classes.
 *
 * @author Eric Butler (edbutler)
 */

#ifndef _VEC_VEC_H_
#define _VEC_VEC_H_

#include "462math.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>


/** A 2D integer vector */
struct ivec2 {
	ivec2() {
		this->x = 0;
		this->y = 0;
	}

	ivec2(int x, int y) {
		this->x = x;
		this->y = y;
	}

	ivec2(const ivec2 &v) {
		this->x = v.x;
		this->y = v.y;
	}

	int x,y;
};


/**
 * A 2d vector.
 */
class Vec2
{
public:
    /**
     * The zero vector.
     */
    static const Vec2 Zero;

    /**
     * The vector (1,1).
     */
    static const Vec2 Ones;

    /**
     * The vector (1,0)
     */
    static const Vec2 UnitX;

    /**
     * The vector (0,1)
     */
    static const Vec2 UnitY;

    /**
     * Components of this vector.
     */
    real_t x, y;

    /**
     * Default constructor. Leaves values unitialized.
     */
    Vec2() {}

    /**
     * Create a vector with the given values.
     */
    Vec2(real_t x, real_t y)
        : x(x), y(y) {}

    /**
     * Create a vector from the elements of the given array.
     */
    explicit Vec2(const float p[2])
        : x(p[0]), y(p[1]) {}

    /**
     * Create a vector from the elements of the given array.
     */
    explicit Vec2(const double p[2])
        : x(p[0]), y(p[1]) {}

    // also uses default copy and assignment

    Vec2 operator+(const Vec2& rhs) const
    {
        return Vec2( x + rhs.x, y + rhs.y );
    }

    Vec2& operator+=(const Vec2& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Vec2 operator-(const Vec2& rhs) const
    {
        return Vec2( x - rhs.x, y - rhs.y );
    }

    Vec2& operator-=(const Vec2& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    // not really mathematically sound, but makes sense for colors
    Vec2 operator*(const Vec2& rhs) const
    {
        return Vec2( x * rhs.x, y * rhs.y );
    }

    // not really mathematically sound, but makes sense for colors
    Vec2& operator*=(const Vec2& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        return *this;
    }

    Vec2 operator*(real_t s) const
    {
        return Vec2( x * s, y * s );
    }

    Vec2& operator*=(real_t s)
    {
        x *= s;
        y *= s;
        return *this;
    }

    // not really mathematically sound, but makes sense for colors
    Vec2 operator/(const Vec2& rhs) const
    {
        return Vec2( x / rhs.x, y / rhs.y );
    }

    // not really mathematically sound, but makes sense for colors
    Vec2& operator/=(const Vec2& rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        return *this;
    }

    Vec2 operator/(real_t s) const
    {
        return Vec2( x / s, y / s );
    }

    Vec2& operator/=(real_t s)
    {
        x /= s;
        y /= s;
        return *this;
    }

    Vec2 operator-() const
    {
        return Vec2( -x, -y );
    }

    /**
     * @remark No bounds checking.
     */
    real_t operator[](size_t i) const
    {
        return (&x)[i];
    }

    /**
     * @remark No bounds checking.
     */
    real_t& operator[](size_t i)
    {
        return (&x)[i];
    }

    bool operator==(const Vec2& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }

    bool operator!=(const Vec2& rhs) const
    {
        return !operator==(rhs);
    }

    /**
     * Returns the dot product of two vectors
     */
    real_t dot(const Vec2& rhs) const
    {
        return x*rhs.x + y*rhs.y;
    }

    /**
     * Returns the magnitude of a vector
     */
    real_t magnitude() const
    {
        return sqrt(x*x + y*y);
    }

    /**
     * Efficiency function: does not require square root operation.
     */
    real_t squared_magnitude() const
    {
        return x*x + y*y;
    }

    /**
     * Calculate the positive distance between two vectors.
     */
    real_t distance(const Vec2& rhs) const
    {
        return (*this-rhs).magnitude();
    }

    /**
     * Efficiency function: does not require square root operation.
     */
    real_t squared_distance(const Vec2& rhs) const
    {
        return (*this-rhs).squared_magnitude();
    }

    /**
     * Returns the unit vector pointing in the same direction as this vector.
     */
    Vec2 unit() const
    {
        return *this / magnitude();
    }

    /**
     * Noramlizes this vector; that is, sets its magnitude to 1.
     */
    Vec2& normalize()
    {
        return *this /= magnitude();
    }

    /**
     * Returns a vector whose elements are the absolute values of all the
     * elements of this vector.
     */
    Vec2 abs() const
    {
        return Vec2(fabs(x), fabs(y));
    }

    /**
     * Returns a vector which is the point exactly between this and the given
     * vector.
     */
    Vec2 midpoint(const Vec2& rhs) const
    {
        return (*this + rhs) * .5;
    }

    /**
     * Clamps the lower bound of this vector; that is, sets this vector's values
     * to the max of the current values and the given vector's values.
     * @param rhs The vector representing the desired lower bound.
     */
    Vec2& clamp_min(const Vec2& rhs)
    {
        x = std::max(x, rhs.x);
        y = std::max(y, rhs.y);
        return *this;
    }

    /**
     * Clamps the upper bound of this vector; that is, sets this vector's values
     * to the min of the current values and the given vector's values.
     * @param rhs The vector representing the desired upper bound.
     */
    Vec2& clamp_max(const Vec2& rhs)
    {
        x = std::min(x, rhs.x);
        y = std::min(y, rhs.y);
        return *this;
    }

    /**
     * Returns a vector whose values are the maximum of this vector's values and
     * the given vector's values.
     */
    Vec2 maximum(const Vec2& rhs) const
    {
        return Vec2(
            std::max(x, rhs.x),
            std::max(y, rhs.y)
        );
    }

    /**
     * Returns a vector whose values are the minimum of this vector's values and
     * the given vector's values.
     */
    Vec2 minimum(const Vec2& rhs) const
    {
        return Vec2(
            std::min(x, rhs.x),
            std::min(y, rhs.y)
        );
    }
};

inline Vec2 operator*(real_t s, const Vec2& rhs)
{
    return rhs * s;
}

/**
 * Outputs a vector text formatted as "(x,y,z)".
 */
std::ostream& operator<<(std::ostream& os, const Vec2& rhs);

/**
 * Reads in a vector text formatted as "(x,y,z)". Whitespaces are fine, as is
 * using '<' and '>' instead of '(' and ')'.
 */
std::istream& operator>>(std::istream& is, Vec2& rhs);

/**
 * A 3d vector.
 */
class Vec3
{
public:
    /**
     * The zero vector.
     */
    static const Vec3 Zero;

    /**
     * The vector (1,1,1).
     */
    static const Vec3 Ones;

    /**
     * The vector (1,0,0)
     */
    static const Vec3 UnitX;

    /**
     * The vector (0,1,0)
     */
    static const Vec3 UnitY;

    /**
     * The vector (0,0,1)
     */
    static const Vec3 UnitZ;

    /**
     * Components of this vector.
     */
    real_t x, y, z;

    /**
     * Default constructor. Leaves values unitialized.
     */
    Vec3() {}

    /**
     * Create a vector with the given values.
     */
    Vec3(real_t x, real_t y, real_t z)
        : x(x), y(y), z(z) {}

    /**
     * Create a vector from the elements of the given array.
     */
    explicit Vec3(const float p[3])
        : x(p[0]), y(p[1]), z(p[2]) {}

    /**
     * Create a vector from the elements of the given array.
     */
    explicit Vec3(const double p[3])
        : x(p[0]), y(p[1]), z(p[2]) {}

    // also uses default copy and assignment

    Vec3 operator+(const Vec3& rhs) const
    {
        return Vec3( x + rhs.x, y + rhs.y, z + rhs.z );
    }

    Vec3& operator+=(const Vec3& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    Vec3 operator-(const Vec3& rhs) const
    {
        return Vec3( x - rhs.x, y - rhs.y, z - rhs.z );
    }

    Vec3& operator-=(const Vec3& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    // not really mathematically sound, but makes sense for colors
    Vec3 operator*(const Vec3& rhs) const
    {
        return Vec3( x * rhs.x, y * rhs.y, z * rhs.z );
    }

    // not really mathematically sound, but makes sense for colors
    Vec3& operator*=(const Vec3& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }

    Vec3 operator*(real_t s) const
    {
        return Vec3( x * s, y * s, z * s );
    }

    Vec3& operator*=(real_t s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    // not really mathematically sound, but makes sense for colors
    Vec3 operator/(const Vec3& rhs) const
    {
        return Vec3( x / rhs.x, y / rhs.y, z / rhs.z );
    }

    // not really mathematically sound, but makes sense for colors
    Vec3& operator/=(const Vec3& rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }

    Vec3 operator/(real_t s) const
    {
        return Vec3( x / s, y / s, z / s );
    }

    Vec3& operator/=(real_t s)
    {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }

    Vec3 operator-() const
    {
        return Vec3( -x, -y, -z);
    }

    /**
     * @remark No bounds checking.
     */
    real_t operator[](size_t i) const
    {
        return (&x)[i];
    }

    /**
     * @remark No bounds checking.
     */
    real_t& operator[](size_t i)
    {
        return (&x)[i];
    }

    bool operator==(const Vec3& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }

    bool operator!=(const Vec3& rhs) const
    {
        return !operator==(rhs);
    }

    /**
     * Returns the dot product of two vectors
     */
    real_t dot(const Vec3& rhs) const
    {
        return x*rhs.x + y*rhs.y + z*rhs.z;
    }

    /**
     * Returns the cross product of two vectors
     */
    Vec3 cross(const Vec3& rhs) const
    {
        return Vec3(
            y*rhs.z - z*rhs.y,
            z*rhs.x - x*rhs.z,
            x*rhs.y - y*rhs.x );
    }

    /**
     * Returns the magnitude of a vector
     */
    real_t magnitude() const
    {
        return sqrt(x*x + y*y + z*z);
    }

    /**
     * Efficiency function: does not require square root operation.
     */
    real_t squared_magnitude() const
    {
        return x*x + y*y + z*z;
    }

    /**
     * Calculate the positive distance between two vectors.
     */
    real_t distance(const Vec3& rhs) const
    {
        return (*this-rhs).magnitude();
    }

    /**
     * Efficiency function: does not require square root operation.
     */
    real_t squared_distance(const Vec3& rhs) const
    {
        return (*this-rhs).squared_magnitude();
    }

    /**
     * Returns the unit vector pointing in the same direction as this vector.
     */
    Vec3 unit() const
    {
        return *this / magnitude();
    }

    /**
     * Noramlizes this vector; that is, sets its magnitude to 1.
     */
    Vec3& normalize()
    {
        return *this /= magnitude();
    }

    /**
     * Returns a vector whose elements are the absolute values of all the
     * elements of this vector.
     */
    Vec3 abs() const
    {
        return Vec3(fabs(x), fabs(y), fabs(z));
    }

    /**
     * Returns a vector which is the point exactly between this and the given
     * vector.
     */
    Vec3 midpoint(const Vec3& rhs) const
    {
        return (*this + rhs) * .5;
    }

    /**
     * Clamps the lower bound of this vector; that is, sets this vector's values
     * to the max of the current values and the given vector's values.
     * @param rhs The vector representing the desired lower bound.
     */
    Vec3& clamp_min(const Vec3& rhs)
    {
        x = std::max(x, rhs.x);
        y = std::max(y, rhs.y);
        z = std::max(z, rhs.z);
        return *this;
    }

    /**
     * Clamps the upper bound of this vector; that is, sets this vector's values
     * to the min of the current values and the given vector's values.
     * @param rhs The vector representing the desired upper bound.
     */
    Vec3& clamp_max(const Vec3& rhs)
    {
        x = std::min(x, rhs.x);
        y = std::min(y, rhs.y);
        z = std::min(z, rhs.z);
        return *this;
    }

    /**
     * Returns a vector whose values are the maximum of this vector's values and
     * the given vector's values.
     */
    Vec3 maximum(const Vec3& rhs) const
    {
        return Vec3(
            std::max(x, rhs.x),
            std::max(y, rhs.y),
            std::max(z, rhs.z)
        );
    }

    /**
     * Returns a vector whose values are the minimum of this vector's values and
     * the given vector's values.
     */
    Vec3 minimum(const Vec3& rhs) const
    {
        return Vec3(
            std::min(x, rhs.x),
            std::min(y, rhs.y),
            std::min(z, rhs.z)
        );
    }

    void to_array(float arr[3]) const
    {
        arr[0] = float(x);
        arr[1] = float(y);
        arr[2] = float(z);
    }
};

inline Vec3 operator*(real_t s, const Vec3& rhs)
{
    return rhs * s;
}

/**
 * Outputs a vector text formatted as "(x,y,z)".
 */
std::ostream& operator<<(std::ostream& os, const Vec3& rhs);

/**
 * Reads in a vector text formatted as "(x,y,z)". Whitespaces are fine, as is
 * using '<' and '>' instead of '(' and ')'.
 */
std::istream& operator>>(std::istream& is, Vec3& rhs);

/**
 * A 4d vector.
 */
class Vec4
{
public:
    /**
     * The zero vector.
     */
    static const Vec4 Zero;

    /**
     * The vector (1,1,1,1).
     */
    static const Vec4 Ones;

    /**
     * The vector (1,0,0,0)
     */
    static const Vec4 UnitX;

    /**
     * The vector (0,1,0,0)
     */
    static const Vec4 UnitY;

    /**
     * The vector (0,0,1,0)
     */
    static const Vec4 UnitZ;

    /**
     * The vector (0,0,0,1)
     */
    static const Vec4 UnitW;

    /**
     * Components of this vector.
     */
    real_t x, y, z, w;

    /**
     * Default constructor. Leaves values unitialized.
     */
    Vec4() {}

    /**
     * Create a vector with the given values.
     */
    Vec4(real_t x, real_t y, real_t z, real_t w)
        :x(x), y(y), z(z), w(w) {}

    /**
     * Create the vector (v.x, v.y, v.z, w).
     */
    Vec4(const Vec3& v, real_t w)
        :x(v.x), y(v.y), z(v.z), w(w) {}

    /**
     * Create a vector from the elements of the given array.
     */
    explicit Vec4(const float p[4])
        : x(p[0]), y(p[1]), z(p[2]), w(p[3]) {}

    /**
     * Create a vector from the elements of the given array.
     */
    explicit Vec4(const double p[4])
        : x(p[0]), y(p[1]), z(p[2]), w(p[3]) {}

    // also uses default copy and assignment

    Vec4 operator+(const Vec4& rhs) const
    {
        return Vec4( x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w );
    }

    Vec4& operator+=(const Vec4& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;
        return *this;
    }

    Vec4 operator-(const Vec4& rhs) const
    {
        return Vec4( x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w );
    }

    Vec4& operator-=(const Vec4& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
        return *this;
    }

    // not really mathematically sound, but makes sense for colors
    Vec4 operator*(const Vec4& rhs) const
    {
        return Vec4( x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w );
    }

    // not really mathematically sound, but makes sense for colors
    Vec4& operator*=(const Vec4& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        w *= rhs.w;
        return *this;
    }

    Vec4 operator*(real_t s) const
    {
        return Vec4( x * s, y * s, z * s, w * s );
    }

    Vec4& operator*=(real_t s)
    {
        x *= s;
        y *= s;
        z *= s;
        w *= s;
        return *this;
    }

    // not really mathematically sound, but makes sense for colors
    Vec4 operator/(const Vec4& rhs) const
    {
        return Vec4( x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w );
    }

    // not really mathematically sound, but makes sense for colors
    Vec4& operator/=(const Vec4& rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        w /= rhs.w;
        return *this;
    }

    Vec4 operator/(real_t s) const
    {
        return Vec4( x / s, y / s, z / s, w / s );
    }

    Vec4& operator/=(real_t s)
    {
        x /= s;
        y /= s;
        z /= s;
        w /= s;
        return *this;
    }

    Vec4 operator-() const
    {
        return Vec4( -x, -y, -z, -w);
    }

    /**
     * @remark No bounds checking.
     */
    real_t operator[](size_t i) const
    {
        return (&x)[i];
    }

    /**
     * @remark No bounds checking.
     */
    real_t& operator[](size_t i)
    {
        return (&x)[i];
    }

    bool operator==(const Vec4& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }

    bool operator!=(const Vec4& rhs) const
    {
        return !operator==(rhs);
    }

    /**
     * Returns the 3d vector corresponding to this 4d vector.
     * @remark If w==0, returns (x,y,z).
     */
    Vec3 projection() const
    {
        real_t tmpw = w == 0 ? 1 : w;
        return Vec3( x/tmpw, y/tmpw, z/tmpw );
    }

    /**
     * Returns the first three components, ignoring the fourth
     */
    Vec3 xyz() const
    {
        return Vec3(x, y, z);
    }

    /**
     * Returns the dot product of two vectors
     */
    real_t dot(const Vec4& rhs) const
    {
        return x*rhs.x + y*rhs.y + z*rhs.z + w*rhs.w;
    }

    /**
     * Returns the magnitude of a vector
     */
    real_t magnitude() const
    {
        return sqrt(x*x + y*y + z*z + w*w);
    }

    /**
     * Efficiency function: does not require square root operation.
     */
    real_t squared_magnitude() const
    {
        return x*x + y*y + z*z + w*w;
    }

    /**
     * Calculate the positive distance between two vectors.
     */
    real_t distance(const Vec4& rhs) const
    {
        return (*this-rhs).magnitude();
    }

    /**
     * Efficiency function: does not require square root operation.
     */
    real_t squared_distance(const Vec4& rhs) const
    {
        return (*this-rhs).squared_magnitude();
    }

    /**
     * Returns the unit vector pointing in the same direction as this vector.
     */
    Vec4 unit() const
    {
        return *this / magnitude();
    }

    /**
     * Noramlizes this vector; that is, sets its magnitude to 1.
     */
    Vec4& normalize()
    {
        return *this /= magnitude();
    }

    /**
     * Returns a vector whose elements are the absolute values of all the
     * elements of this vector.
     */
    Vec4 abs() const
    {
        return Vec4(fabs(x), fabs(y), fabs(z), fabs(w));
    }

    /**
     * Returns a vector which is the point exactly between this and the given
     * vector.
     */
    Vec4 midpoint(const Vec4& rhs) const
    {
        return (*this + rhs) * .5;
    }

    /**
     * Clamps the lower bound of this vector; that is, sets this vector's values
     * to the max of the current values and the given vector's values.
     * @param rhs The vector representing the desired lower bound.
     */
    Vec4& clamp_min(const Vec4& rhs)
    {
        x = std::max(x, rhs.x);
        y = std::max(y, rhs.y);
        z = std::max(z, rhs.z);
        w = std::max(w, rhs.w);
        return *this;
    }

    /**
     * Clamps the upper bound of this vector; that is, sets this vector's values
     * to the min of the current values and the given vector's values.
     * @param rhs The vector representing the desired upper bound.
     */
    Vec4& clamp_max(const Vec4& rhs)
    {
        x = std::min(x, rhs.x);
        y = std::min(y, rhs.y);
        z = std::min(z, rhs.z);
        w = std::min(w, rhs.w);
        return *this;
    }

    /**
     * Returns a vector whose values are the maximum of this vector's values and
     * the given vector's values.
     */
    Vec4 maximum(const Vec4& rhs) const
    {
        return Vec4(
            std::max(x, rhs.x),
            std::max(y, rhs.y),
            std::max(z, rhs.z),
            std::max(w, rhs.w)
        );
    }

    /**
     * Returns a vector whose values are the minimum of this vector's values and
     * the given vector's values.
     */
    Vec4 minimum(const Vec4& rhs) const
    {
        return Vec4(
            std::min(x, rhs.x),
            std::min(y, rhs.y),
            std::min(z, rhs.z),
            std::min(w, rhs.w)
        );
    }

    void to_array(float arr[4]) const
    {
        arr[0] = float(x);
        arr[1] = float(y);
        arr[2] = float(z);
        arr[3] = float(w);
    }
};

inline Vec4 operator*(real_t s, const Vec4& rhs)
{
    return rhs * s;
}

/**
 * Outputs a vector text formatted as "(x,y,z)".
 */
std::ostream& operator<<(std::ostream& os, const Vec4& rhs);

/**
 * Reads in a vector text formatted as "(x,y,z)". Whitespaces are fine, as is
 * using '<' and '>' instead of '(' and ')'.
 */
std::istream& operator>>(std::istream& is, Vec4& rhs);

#endif /* _VEC_VEC_H_ */

