/**
 * @file 462math.h
 * @brief General math declarations and definitions.
 *
 * @author Zeyang Li (zeyangl)
 * @author Eric Butler (edbutler)
 */

#ifndef _462MATH_H_
#define _462MATH_H_

#define REAL_IS_DOUBLE 0

#if REAL_IS_DOUBLE
typedef double real_t;
#define glLoadMatrixr glLoadMatrixd
#define glMultMatrixr glMultMatrixd
#else
typedef float real_t;
#define glLoadMatrixr glLoadMatrixf
#define glMultMatrixr glMultMatrixf
#pragma warning(disable: 4244)
#pragma warning(disable: 4305)
#endif

#define PI (real_t)(3.141592653589793238)

class Vec4;

void color_vector_to_array(const Vec4& color, unsigned char* arr);
Vec4 color_array_to_vector(const unsigned char* arr);

#endif /* _462MATH_H_ */

