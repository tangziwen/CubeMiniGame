#include "Matrix44.h"

#include "string.h"
#include "math.h"
namespace tzw {

Matrix44::Matrix44()
{
    setToIdentity();
}

void Matrix44::setToIdentity()
{
    memset(m_data,0,sizeof(m_data));
    m_data[0] = 1;
    m_data[5] = 1;
    m_data[10] = 1;
    m_data[15] = 1;
}

void Matrix44::translate(vec3 offset)
{
    m_data[12] = offset.x;
    m_data[13] = offset.y;
    m_data[14] = offset.z;
}

void Matrix44::scale(vec3 scaleFactor)
{
    m_data[0] = scaleFactor.x;
    m_data[5] = scaleFactor.y;
    m_data[10] = scaleFactor.z;
}

void Matrix44::rotate(const Quaternion &quaternion)
{
    float x = quaternion.x, y = quaternion.y, z = quaternion.z,w = quaternion.w,
            x2 = x + x,
            y2 = y + y,
            z2 = z + z,

            xx = x * x2,
            xy = x * y2,
            xz = x * z2,
            yy = y * y2,
            yz = y * z2,
            zz = z * z2,
            wx = w * x2,
            wy = w * y2,
            wz = w * z2;

    m_data[0] = 1 - (yy + zz);
    m_data[1] = xy + wz;
    m_data[2] = xz - wy;
    m_data[3] = 0;

    m_data[4] = xy - wz;
    m_data[5] = 1 - (xx + zz);
    m_data[6] = yz + wx;
    m_data[7] = 0;

    m_data[8] = xz + wy;
    m_data[9] = yz - wx;
    m_data[10] = 1 - (xx + yy);
    m_data[11] = 0;

    m_data[12] = 0;
    m_data[13] = 0;
    m_data[14] = 0;
    m_data[15] = 1;
}

Matrix44 Matrix44::operator *(const Matrix44 &other) const
{
    Matrix44 dest;
    auto mat2 = other.m_data;
    // Cache the matrix values (makes for huge speed increases!)
    float a00 = m_data[0], a01 = m_data[1], a02 = m_data[2], a03 = m_data[3],
            a10 = m_data[4], a11 = m_data[5], a12 = m_data[6], a13 = m_data[7],
            a20 = m_data[8], a21 = m_data[9], a22 = m_data[10], a23 = m_data[11],
            a30 = m_data[12], a31 = m_data[13], a32 = m_data[14], a33 = m_data[15],

            b00 = mat2[0], b01 = mat2[1], b02 = mat2[2], b03 = mat2[3],
            b10 = mat2[4], b11 = mat2[5], b12 = mat2[6], b13 = mat2[7],
            b20 = mat2[8], b21 = mat2[9], b22 = mat2[10], b23 = mat2[11],
            b30 = mat2[12], b31 = mat2[13], b32 = mat2[14], b33 = mat2[15];

    dest.m_data[0] = b00 * a00 + b01 * a10 + b02 * a20 + b03 * a30;
    dest.m_data[1] = b00 * a01 + b01 * a11 + b02 * a21 + b03 * a31;
    dest.m_data[2] = b00 * a02 + b01 * a12 + b02 * a22 + b03 * a32;
    dest.m_data[3] = b00 * a03 + b01 * a13 + b02 * a23 + b03 * a33;
    dest.m_data[4] = b10 * a00 + b11 * a10 + b12 * a20 + b13 * a30;
    dest.m_data[5] = b10 * a01 + b11 * a11 + b12 * a21 + b13 * a31;
    dest.m_data[6] = b10 * a02 + b11 * a12 + b12 * a22 + b13 * a32;
    dest.m_data[7] = b10 * a03 + b11 * a13 + b12 * a23 + b13 * a33;
    dest.m_data[8] = b20 * a00 + b21 * a10 + b22 * a20 + b23 * a30;
    dest.m_data[9] = b20 * a01 + b21 * a11 + b22 * a21 + b23 * a31;
    dest.m_data[10] = b20 * a02 + b21 * a12 + b22 * a22 + b23 * a32;
    dest.m_data[11] = b20 * a03 + b21 * a13 + b22 * a23 + b23 * a33;
    dest.m_data[12] = b30 * a00 + b31 * a10 + b32 * a20 + b33 * a30;
    dest.m_data[13] = b30 * a01 + b31 * a11 + b32 * a21 + b33 * a31;
    dest.m_data[14] = b30 * a02 + b31 * a12 + b32 * a22 + b33 * a32;
    dest.m_data[15] = b30 * a03 + b31 * a13 + b32 * a23 + b33 * a33;
    return dest;
}

vec4 Matrix44::operator *(const vec4 &other) const
{
    vec4 result;
    float x = other.x, y = other.y, z = other.z,w = other.w;
    auto mat = m_data;
    result.x = mat[0] * x + mat[4] * y + mat[8] * z + mat[12] * w;
    result.y = mat[1] * x + mat[5] * y + mat[9] * z + mat[13] * w;
    result.z = mat[2] * x + mat[6] * y + mat[10] * z + mat[14] * w;
    result.w = mat[3] * x + mat[7] * y + mat[11] * z + mat[15] * w;
    return result;
}

void Matrix44::ortho(float left, float right, float bottom, float top, float near, float far)
{
    float rl = (right - left),
            tb = (top - bottom),
            fn = (far - near);
    m_data[0] = 2 / rl;
    m_data[1] = 0;
    m_data[2] = 0;
    m_data[3] = 0;
    m_data[4] = 0;
    m_data[5] = 2 / tb;
    m_data[6] = 0;
    m_data[7] = 0;
    m_data[8] = 0;
    m_data[9] = 0;
    m_data[10] = -2 / fn;
    m_data[11] = 0;
    m_data[12] = -(left + right) / rl;
    m_data[13] = -(top + bottom) / tb;
    m_data[14] = -(far + near) / fn;
    m_data[15] = 1;
}

void Matrix44::perspective(float fovy, float aspect, float near, float far)
{
    float top = near * float(tan(fovy * 3.14159265358979323846 / 360.0)),
            right = top * aspect;
    frustum(-right, right, -top, top, near, far);
}

Matrix44 Matrix44::inverted(bool *invertible)
{
    auto mat = m_data;
    Matrix44 result;
    float a00 = mat[0], a01 = mat[1], a02 = mat[2], a03 = mat[3],
            a10 = mat[4], a11 = mat[5], a12 = mat[6], a13 = mat[7],
            a20 = mat[8], a21 = mat[9], a22 = mat[10], a23 = mat[11],
            a30 = mat[12], a31 = mat[13], a32 = mat[14], a33 = mat[15],

            b00 = a00 * a11 - a01 * a10,
            b01 = a00 * a12 - a02 * a10,
            b02 = a00 * a13 - a03 * a10,
            b03 = a01 * a12 - a02 * a11,
            b04 = a01 * a13 - a03 * a11,
            b05 = a02 * a13 - a03 * a12,
            b06 = a20 * a31 - a21 * a30,
            b07 = a20 * a32 - a22 * a30,
            b08 = a20 * a33 - a23 * a30,
            b09 = a21 * a32 - a22 * a31,
            b10 = a21 * a33 - a23 * a31,
            b11 = a22 * a33 - a23 * a32,

            d = (b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06),
            invDet;

    // Calculate the determinant
    if (!d )
    {
        if (invertible) {(*invertible) = false;}
        return result;
    }
    else
    {
        if (invertible) {(*invertible) = true;}
    }
    invDet = 1 / d;

    result.m_data[0] = (a11 * b11 - a12 * b10 + a13 * b09) * invDet;
    result.m_data[1] = (-a01 * b11 + a02 * b10 - a03 * b09) * invDet;
    result.m_data[2] = (a31 * b05 - a32 * b04 + a33 * b03) * invDet;
    result.m_data[3] = (-a21 * b05 + a22 * b04 - a23 * b03) * invDet;
    result.m_data[4] = (-a10 * b11 + a12 * b08 - a13 * b07) * invDet;
    result.m_data[5] = (a00 * b11 - a02 * b08 + a03 * b07) * invDet;
    result.m_data[6] = (-a30 * b05 + a32 * b02 - a33 * b01) * invDet;
    result.m_data[7] = (a20 * b05 - a22 * b02 + a23 * b01) * invDet;
    result.m_data[8] = (a10 * b10 - a11 * b08 + a13 * b06) * invDet;
    result.m_data[9] = (-a00 * b10 + a01 * b08 - a03 * b06) * invDet;
    result.m_data[10] = (a30 * b04 - a31 * b02 + a33 * b00) * invDet;
    result.m_data[11] = (-a20 * b04 + a21 * b02 - a23 * b00) * invDet;
    result.m_data[12] = (-a10 * b09 + a11 * b07 - a12 * b06) * invDet;
    result.m_data[13] = (a00 * b09 - a01 * b07 + a02 * b06) * invDet;
    result.m_data[14] = (-a30 * b03 + a31 * b01 - a32 * b00) * invDet;
    result.m_data[15] = (a20 * b03 - a21 * b01 + a22 * b00) * invDet;
    return result;
}

Matrix44 Matrix44::transpose()
{
    Matrix44 result;
    auto dest = result.m_data;
    auto mat = m_data;
    dest[0] = mat[0];
    dest[1] = mat[4];
    dest[2] = mat[8];
    dest[3] = mat[12];
    dest[4] = mat[1];
    dest[5] = mat[5];
    dest[6] = mat[9];
    dest[7] = mat[13];
    dest[8] = mat[2];
    dest[9] = mat[6];
    dest[10] = mat[10];
    dest[11] = mat[14];
    dest[12] = mat[3];
    dest[13] = mat[7];
    dest[14] = mat[11];
    dest[15] = mat[15];
    return result;
}

vec4 Matrix44::operator *(const vec4 &v)
{
    vec4 dst;
    auto m = m_data;
    dst.x = v.x * m[0] + v.y * m[4] + v.z * m[8] + v.w * m[12];
    dst.y = v.x * m[1] + v.y * m[5] + v.z * m[9] + v.w * m[13];
    dst.z = v.x * m[2] + v.y * m[6] + v.z * m[10] + v.w * m[14];
    dst.w = v.x * m[3] + v.y * m[7] + v.z * m[11] + v.w * m[15];
    return dst;
}

vec3 Matrix44::transformVec3(vec3 v)
{
    auto result = *this * vec4(v, 1.0f);
    return vec3(result.x, result.y, result.z);
}

void Matrix44::frustum(float left, float right, float bottom, float top, float near, float far)
{
    float rl = (right - left),
            tb = (top - bottom),
            fn = (far - near);
    m_data[0] = (near * 2) / rl;
    m_data[1] = 0;
    m_data[2] = 0;
    m_data[3] = 0;
    m_data[4] = 0;
    m_data[5] = (near * 2) / tb;
    m_data[6] = 0;
    m_data[7] = 0;
    m_data[8] = (right + left) / rl;
    m_data[9] = (top + bottom) / tb;
    m_data[10] = -(far + near) / fn;
    m_data[11] = -1;
    m_data[12] = 0;
    m_data[13] = 0;
    m_data[14] = -(far * near * 2) / fn;
    m_data[15] = 0;
}

float *Matrix44::data()
{
    return m_data;
}

vec3 Matrix44::up()
{
    return vec3(m_data[4], m_data[5], m_data[6]);
}

vec3 Matrix44::forward()
{
    return -vec3(m_data[8], m_data[9], m_data[10]);
}

vec3 Matrix44::right()
{
    return vec3(m_data[0], m_data[1], m_data[2]);
}

} // namespace tzw
