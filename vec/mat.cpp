/**
 * @file mat.cpp
 * @brief Matrix classes.
 *
 * @author Eric Butler (edbutler)
 * @author Zeyang Li (zeyangl)
 */

#include "mat.h"
#include "quat.h"
#include <cstring>

const Mat3 Mat3::Identity = Mat3( 1, 0, 0,
                                  0, 1, 0,
                                  0, 0, 1 );

const Mat3 Mat3::Zero = Mat3( 0, 0, 0,
                              0, 0, 0,
                              0, 0, 0 );


Mat3::Mat3(real_t r[SIZE])
{
    memcpy(m ,r, sizeof r);
}

Mat3::Mat3(real_t m00, real_t m10, real_t m20,
           real_t m01, real_t m11, real_t m21,
           real_t m02, real_t m12, real_t m22)
{
    _m[0][0] = m00;
    _m[1][0] = m10;
    _m[2][0] = m20;
    _m[0][1] = m01;
    _m[1][1] = m11;
    _m[2][1] = m21;
    _m[0][2] = m02;
    _m[1][2] = m12;
    _m[2][2] = m22;
}


Mat3 Mat3::operator+(const Mat3& rhs) const
{
    Mat3 rv;
    for ( int i=0; i<SIZE; i++ )
        rv.m[i] = m[i] + rhs.m[i];
    return rv;
}

Mat3& Mat3::operator+=(const Mat3& rhs)
{
    for ( int i=0; i<SIZE; i++ )
        m[i] += rhs.m[i];
    return *this;
}

Mat3 Mat3::operator-(const Mat3& rhs) const
{
    Mat3 rv;
    for ( int i=0; i<SIZE; i++ )
        rv.m[i] = m[i] - rhs.m[i];
    return rv;
}

Mat3& Mat3::operator-=(const Mat3& rhs)
{
    for ( int i=0; i<SIZE; i++ )
        m[i] -= rhs.m[i];
    return *this;
}

Mat3 Mat3::operator*(const Mat3& rhs) const
{
    Mat3 product;
    for (int i=0; i<DIM; ++i)
        for (int j=0; j<DIM; ++j)
            product._m[i][j] =
                _m[0][j]*rhs._m[i][0] + _m[1][j]*rhs._m[i][1] +
                _m[2][j]*rhs._m[i][2];
    return product;
}

Vec3 Mat3::operator*(const Vec3& v) const
{
    return Vec3( _m[0][0]*v.x + _m[1][0]*v.y + _m[2][0]*v.z,
                 _m[0][1]*v.x + _m[1][1]*v.y + _m[2][1]*v.z,
                 _m[0][2]*v.x + _m[1][2]*v.y + _m[2][2]*v.z );
}

Mat3& Mat3::operator*=(const Mat3& rhs)
{
    return *this = operator*(rhs);
}

Mat3 Mat3::operator*(real_t r) const
{
    Mat3 rv;
    for ( int i=0; i<SIZE; i++ )
        rv.m[i] = m[i] * r;
    return rv;
}

Mat3& Mat3::operator*=(real_t r)
{
    for ( int i=0; i<SIZE; i++ )
        m[i] *= r;
    return *this;
}

Mat3 Mat3::operator/(real_t r) const
{
    Mat3 rv;
    real_t inv = 1/r;
    for ( int i=0; i<SIZE; i++ )
        rv.m[i] = m[i] * inv;
    return rv;
}

Mat3& Mat3::operator/=(real_t r)
{
    real_t inv = 1/r;
    for ( int i=0; i<SIZE; i++ )
        m[i] *= inv;
    return *this;
}

Mat3 Mat3::operator-() const
{
    Mat3 rv;
    for ( int i=0; i<SIZE; i++ )
        rv.m[i] = -m[i];
    return rv;
}

bool Mat3::operator==(const Mat3& rhs) const
{
    return memcmp( m, rhs.m, sizeof m ) == 0;
}

bool Mat3::operator!=(const Mat3& rhs) const
{
    return !operator==(rhs);
}

Mat3 Mat3::transpose() const
{
    return Mat3( _m[0][0], _m[0][1], _m[0][2],
                 _m[1][0], _m[1][1], _m[1][2],
                 _m[2][0], _m[2][1], _m[2][2] );
}

const Mat4 Mat4::Identity = Mat4( 1, 0, 0, 0,
                                  0, 1, 0, 0,
                                  0, 0, 1, 0,
                                  0, 0 ,0, 1 );

const Mat4 Mat4::Zero = Mat4( 0, 0, 0, 0,
                              0, 0, 0, 0,
                              0, 0, 0, 0,
                              0, 0, 0, 0 );


Mat4::Mat4(real_t r[SIZE])
{
    memcpy(m ,r, sizeof r);
}

Mat4::Mat4(real_t m00, real_t m10, real_t m20, real_t m30,
           real_t m01, real_t m11, real_t m21, real_t m31,
           real_t m02, real_t m12, real_t m22, real_t m32,
           real_t m03, real_t m13, real_t m23, real_t m33)
{
    _m[0][0] = m00;
    _m[1][0] = m10;
    _m[2][0] = m20;
    _m[3][0] = m30;
    _m[0][1] = m01;
    _m[1][1] = m11;
    _m[2][1] = m21;
    _m[3][1] = m31;
    _m[0][2] = m02;
    _m[1][2] = m12;
    _m[2][2] = m22;
    _m[3][2] = m32;
    _m[0][3] = m03;
    _m[1][3] = m13;
    _m[2][3] = m23;
    _m[3][3] = m33;
}


Mat4 Mat4::operator+(const Mat4& rhs) const
{
    Mat4 rv;
    for ( int i=0; i<SIZE; i++ )
        rv.m[i] = m[i] + rhs.m[i];
    return rv;
}

Mat4& Mat4::operator+=(const Mat4& rhs)
{
    for ( int i=0; i<SIZE; i++ )
        m[i] += rhs.m[i];
    return *this;
}

Mat4 Mat4::operator-(const Mat4& rhs) const
{
    Mat4 rv;
    for ( int i=0; i<SIZE; i++ )
        rv.m[i] = m[i] - rhs.m[i];
    return rv;
}

Mat4& Mat4::operator-=(const Mat4& rhs)
{
    for ( int i=0; i<SIZE; i++ )
        m[i] -= rhs.m[i];
    return *this;
}

Mat4 Mat4::operator*(const Mat4& rhs) const
{
    Mat4 product;
    for (int i=0; i<DIM; ++i)
        for (int j=0; j<DIM; ++j)
            product._m[i][j] =
                _m[0][j]*rhs._m[i][0] + _m[1][j]*rhs._m[i][1] +
                _m[2][j]*rhs._m[i][2] + _m[3][j]*rhs._m[i][3];
    return product;
}

Vec4 Mat4::operator*(const Vec4& v) const
{
    return Vec4( _m[0][0]*v.x + _m[1][0]*v.y + _m[2][0]*v.z + _m[3][0]*v.w,
                 _m[0][1]*v.x + _m[1][1]*v.y + _m[2][1]*v.z + _m[3][1]*v.w,
                 _m[0][2]*v.x + _m[1][2]*v.y + _m[2][2]*v.z + _m[3][2]*v.w,
                 _m[0][3]*v.x + _m[1][3]*v.y + _m[2][3]*v.z + _m[3][3]*v.w );
}

Mat4& Mat4::operator*=(const Mat4& rhs)
{
    return *this = operator*(rhs);
}

Mat4 Mat4::operator*(real_t r) const
{
    Mat4 rv;
    for ( int i=0; i<SIZE; i++ )
        rv.m[i] = m[i] * r;
    return rv;
}

Mat4& Mat4::operator*=(real_t r)
{
    for ( int i=0; i<SIZE; i++ )
        m[i] *= r;
    return *this;
}

Mat4 Mat4::operator/(real_t r) const
{
    Mat4 rv;
    real_t inv = 1/r;
    for ( int i=0; i<SIZE; i++ )
        rv.m[i] = m[i] * inv;
    return rv;
}

Mat4& Mat4::operator/=(real_t r)
{
    real_t inv = 1/r;
    for ( int i=0; i<SIZE; i++ )
        m[i] *= inv;
    return *this;
}

Mat4 Mat4::operator-() const
{
    Mat4 rv;
    for ( int i=0; i<SIZE; i++ )
        rv.m[i] = -m[i];
    return rv;
}

bool Mat4::operator==(const Mat4& rhs) const
{
    return memcmp( m, rhs.m, sizeof m ) == 0;
}

bool Mat4::operator!=(const Mat4& rhs) const
{
    return !operator==(rhs);
}

Mat4 Mat4::transpose() const
{
    return Mat4( _m[0][0], _m[0][1], _m[0][2], _m[0][3],
                 _m[1][0], _m[1][1], _m[1][2], _m[1][3],
                 _m[2][0], _m[2][1], _m[2][2], _m[2][3],
                 _m[3][0], _m[3][1], _m[3][2], _m[3][3] );
}

Mat4 Mat4::inverse() const
{
    real_t m00 = _m[0][0], m01 = _m[0][1], m02 = _m[0][2], m03 = _m[0][3];
    real_t m10 = _m[1][0], m11 = _m[1][1], m12 = _m[1][2], m13 = _m[1][3];
    real_t m20 = _m[2][0], m21 = _m[2][1], m22 = _m[2][2], m23 = _m[2][3];
    real_t m30 = _m[3][0], m31 = _m[3][1], m32 = _m[3][2], m33 = _m[3][3];

    real_t v0 = m20 * m31 - m21 * m30;
    real_t v1 = m20 * m32 - m22 * m30;
    real_t v2 = m20 * m33 - m23 * m30;
    real_t v3 = m21 * m32 - m22 * m31;
    real_t v4 = m21 * m33 - m23 * m31;
    real_t v5 = m22 * m33 - m23 * m32;

    real_t t00 = + (v5 * m11 - v4 * m12 + v3 * m13);
    real_t t10 = - (v5 * m10 - v2 * m12 + v1 * m13);
    real_t t20 = + (v4 * m10 - v2 * m11 + v0 * m13);
    real_t t30 = - (v3 * m10 - v1 * m11 + v0 * m12);

    real_t invDet = 1 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

    real_t d00 = t00 * invDet;
    real_t d10 = t10 * invDet;
    real_t d20 = t20 * invDet;
    real_t d30 = t30 * invDet;

    real_t d01 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
    real_t d11 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
    real_t d21 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
    real_t d31 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

    v0 = m10 * m31 - m11 * m30;
    v1 = m10 * m32 - m12 * m30;
    v2 = m10 * m33 - m13 * m30;
    v3 = m11 * m32 - m12 * m31;
    v4 = m11 * m33 - m13 * m31;
    v5 = m12 * m33 - m13 * m32;

    real_t d02 = + (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
    real_t d12 = - (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
    real_t d22 = + (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
    real_t d32 = - (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

    v0 = m21 * m10 - m20 * m11;
    v1 = m22 * m10 - m20 * m12;
    v2 = m23 * m10 - m20 * m13;
    v3 = m22 * m11 - m21 * m12;
    v4 = m23 * m11 - m21 * m13;
    v5 = m23 * m12 - m22 * m13;

    real_t d03 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
    real_t d13 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
    real_t d23 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
    real_t d33 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

    return Mat4(
        d00, d01, d02, d03,
        d10, d11, d12, d13,
        d20, d21, d22, d23,
        d30, d31, d32, d33);
}

std::ostream& operator << ( std::ostream& o, const Mat4& m )
{
    o << "Matrix4(";
    for (size_t i = 0; i < 4; ++i)
    {
        o << " row" << (unsigned)i << "{ ";
        for(size_t j = 0; j < 4; ++j)
        {
            o << m._m[i][j] << " ";
        }
        o << "}";
    }
    o << ")";
    return o;
}

