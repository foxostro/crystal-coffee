/**
 * @file mat.h
 * @brief Matrix classes.
 *
 * @author Eric Butler (edbutler)
 * @author Zeyang Li (zeyangl)
 */

#ifndef _VEC_MAT_H_
#define _VEC_MAT_H_

#include "462math.h"
#include "vec.h"

class Quat;

/**
 * A 3x3 matrix.
 *
 * The matrix is in column-vector compatible notation and is
 * stored in memory by column first. So the memory layout of
 * [ A B C ]
 * [ D E F ]
 * [ G H I ]
 * looks like
 * [ m[0] m[3] m[6] ]
 * [ m[1] m[4] m[7] ]
 * [ m[2] m[5] m[8] ]
 *
 * Vectors are treated a column matrices (3x1).
 *
 * NOTE: The notation mij or m[i][j] is the ith COLUMN and the jth ROW.
 *
 * The matrix is meant for use in linear transformations. Transformations must
 * be combined right-to-left, so if you want to apply A then B the C to a vector V,
 * you would use C*B*A*V.
 */
class Mat3
{
public:
    /**
     * The identity matrix.
     */
    static const Mat3 Identity;

    /**
     * The zero matrix.
     */
    static const Mat3 Zero;

    static const int DIM = 3;
    static const int SIZE = DIM*DIM;

    /**
     * The values of this matrix. Named as both a 1d array and a 2d array.
     */
    union{
        real_t m[SIZE];
        real_t _m[DIM][DIM]; // _m[column][row]
    };

    // constructors

    /**
     * Leaves values unitialized.
     */
    Mat3() {}

    /**
     * Construct a matrix from the given array, in COLUMN MAJOR format
     */
    explicit Mat3(real_t r[SIZE]);

    /**
     * Construct a matrix from the given values in ROW MAJOR format (A,B,C,D,E,F,G,H,I).
     */
    Mat3(real_t m00, real_t m10, real_t m20,
         real_t m01, real_t m11, real_t m21,
         real_t m02, real_t m12, real_t m22);

    // basic operations
    Mat3 operator+(const Mat3& rhs) const;
    Mat3& operator+=(const Mat3& rhs);
    Mat3 operator-(const Mat3& rhs) const;
    Mat3& operator-=(const Mat3& rhs);
    Mat3 operator*(const Mat3& rhs) const;
    Vec3 operator*(const Vec3& v) const;
    Mat3& operator*=(const Mat3& rhs);
    Mat3 operator*(real_t r) const;
    Mat3& operator*=(real_t r);
    Mat3 operator/(real_t r) const;
    Mat3& operator/=(real_t r);
    Mat3 operator-() const;

    // comparaisons
    bool operator==(const Mat3& rhs) const;
    bool operator!=(const Mat3& rhs) const;

    // accessors

    /**
     * Mat3(i,j) gives the element at the ith column and jth row.
     */
    real_t operator()(int col, int row) const
    {
        return _m[col][row];
    }

    /**
     * Mat3(i,j) gives the element at the ith column and jth row.
     */
    real_t& operator()(int col, int row)
    {
        return _m[col][row];
    }

    /**
     * Transform the given vector using this matrix
     */
    Vec3 transform(const Vec3& v) const
    {
        return operator*(v);
    }

    /**
     * Combines two transformations into one, with this Matrix being the first
     * to be applied (rhs) and the given matrix the second (lhs). If this matrix
     * is A and the given lhs is B, this function is equivalent to A=B*A.
     */
    Mat3& concatenate(const Mat3& lhs)
    {
        return *this = lhs*(*this);
    }

    /**
     * Returns the transpose matrix.
     */
    Mat3 transpose() const;

    //Mat3 inverse() const;
};

inline Mat3 operator*(real_t r, const Mat3& m)
{
    return m * r;
}

/**
 * A 4x4 matrix.
 *
 * The matrix is in column-vector compatible notation and is
 * stored in memory by column first. So the memory layout of
 * [ A B C D ]
 * [ E F G H ]
 * [ I J K L ]
 * [ M N O P ]
 * looks like
 * [ m[0] m[4] m[ 8] m[12] ]
 * [ m[1] m[5] m[ 9] m[13] ]
 * [ m[2] m[6] m[10] m[14] ]
 * [ m[3] m[7] m[11] m[15] ]
 *
 * Vectors are treated a column matrices (4x1).
 *
 * NOTE: The notation mij or m[i][j] is the ith COLUMN and the jth ROW.
 *
 * The matrix is meant for use in linear transformations. Transformations must
 * be combined right-to-left, so if you want to apply A then B the C to a vector V,
 * you would use C*B*A*V.
 */
class Mat4
{
public:
    /**
     * The identity matrix.
     */
    static const Mat4 Identity;

    /**
     * The zero matrix.
     */
    static const Mat4 Zero;

    static const int DIM = 4;
    static const int SIZE = DIM*DIM;

    /**
     * The values of this matrix. Named as both a 1d array and a 2d array.
     */
    union{
        real_t m[SIZE];
        real_t _m[DIM][DIM]; // _m[column][row]
    };

    // constructors

    /**
     * Leaves values unitialized.
     */
    Mat4() {}

    /**
     * Construct a matrix from the given array, in COLUMN MAJOR format
     */
    explicit Mat4(real_t r[SIZE]);

    /**
     * Construct a matrix from the given values in ROW MAJOR format (A,B,C,D,E,F,G,H,I).
     */
    Mat4(real_t m00, real_t m10, real_t m20, real_t m30,
         real_t m01, real_t m11, real_t m21, real_t m31,
         real_t m02, real_t m12, real_t m22, real_t m32,
         real_t m03, real_t m13, real_t m23, real_t m33);

    // basic operations
    Mat4 operator+(const Mat4& rhs) const;
    Mat4& operator+=(const Mat4& rhs);
    Mat4 operator-(const Mat4& rhs) const;
    Mat4& operator-=(const Mat4& rhs);
    Mat4 operator*(const Mat4& rhs) const;
    Vec4 operator*(const Vec4& v) const;
    Mat4& operator*=(const Mat4& rhs);
    Mat4 operator*(real_t r) const;
    Mat4& operator*=(real_t r);
    Mat4 operator/(real_t r) const;
    Mat4& operator/=(real_t r);
    Mat4 operator-() const;

    // comparaisons
    bool operator==(const Mat4& rhs) const;
    bool operator!=(const Mat4& rhs) const;

    // accessors

    /**
     * Mat4(i,j) gives the element at the ith column and jth row.
     */
    real_t operator()(int col, int row) const
    {
        return _m[col][row];
    }

    /**
     * Mat4(i,j) gives the element at the ith column and jth row.
     */
    real_t& operator()(int col, int row)
    {
        return _m[col][row];
    }

    /**
     * Transform the given vector using this matrix
     */
    Vec4 transform(const Vec4& v) const
    {
        return operator*(v);
    }

    Vec3 transform_point(const Vec3& v) const
    {
        return transform(Vec4(v, 1)).projection();
    }

    Vec3 transform_vector(const Vec3& v) const
    {
        return transform(Vec4(v, 0)).projection();
    }

    /**
     * Combines two transformations into one, with this Matrix being the first
     * to be applied (rhs) and the given matrix the second (lhs). If this matrix
     * is A and the given lhs is B, this function is equivalent to A=B*A.
     */
    Mat4& concatenate(const Mat4& lhs)
    {
        return *this = lhs*(*this);
    }

    /**
     * Returns the transpose matrix.
     */
    Mat4 transpose() const;

    Mat4 inverse() const;

	static Mat4 perspective(real_t fovy,
	                        real_t aspect,
	                        real_t zNear,
	                        real_t zFar)

	{
		const real_t f = 1.0 / tan(fovy / 2.0);

		return Mat4(f/aspect,   0.0,   0.0,                       0.0,
		            0.0,        f,     0.0,                       0.0,
		            0.0,        0.0,   (zFar+zNear)/(zNear-zFar), (2.0*zFar*zNear)/(zNear-zFar),
					0.0,        0.0,   -1.0,                      0.0);
	}

	static Mat4 ortho(real_t left,
	                  real_t right,
	                  real_t bottom,
				      real_t top,
					  real_t nearVal,
					  real_t farVal)

	{
		return Mat4(2.0 / (right - left), 0.0,                  0.0,                       -(right+left)/(right-left),
		            0.0,                  2.0 / (top - bottom), 0.0,                       -(top+bottom)/(top-bottom),
		            0.0,                  0.0,                  -2.0 / (farVal - nearVal), -(farVal+nearVal)/(farVal-nearVal),
					0.0,                  0.0,                  0.0,                       1.0);
	}

	static Mat4 ortho2D(real_t left,
	                    real_t right,
	                    real_t bottom,
				        real_t top)

	{
		return ortho(left, right, top, bottom, -1, +1);
	}
};

inline Mat4 operator*(real_t r, const Mat4& m)
{
    return m * r;
}

#endif /* _VEC_MAT_H_ */

