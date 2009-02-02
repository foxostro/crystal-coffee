/**
 * @file 462math.cpp
 * @brief General math function definitions.
 *
 * @author Eric Butler (edbutler)
 */

#include "462math.h"
#include "vec.h"
#include <cmath>

void color_vector_to_array(const Vec4& color, unsigned char* arr)
{
    // clamp values
    Vec4 tmp = color;
    tmp.clamp_min(Vec4::Zero);
    tmp.clamp_max(Vec4::Ones);

    // convert to ints
    arr[0] = static_cast<unsigned char>(tmp.x * 0xff);
    arr[1] = static_cast<unsigned char>(tmp.y * 0xff);
    arr[2] = static_cast<unsigned char>(tmp.z * 0xff);
    arr[3] = static_cast<unsigned char>(tmp.w * 0xff);
}

Vec4 color_array_to_vector(const unsigned char* arr)
{
    return Vec4(arr[0] / 255.0, arr[1] / 255.0, arr[2] / 255.0, arr[3] / 255.0);
}

