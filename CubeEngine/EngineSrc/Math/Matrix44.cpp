#include "Matrix44.h"

#include "string.h"
#include <cmath>
#define MATH_TOLERANCE              2e-37f
#define MATH_EPSILON                0.000001f
#include "Engine/EngineDef.h"
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

void Matrix44::setTranslate(const vec3 offset)
{
    m_data[12] = offset.x;
    m_data[13] = offset.y;
    m_data[14] = offset.z;
}

void Matrix44::setScale(vec3 scaleFactor)
{
    m_data[0] = scaleFactor.x;
    m_data[5] = scaleFactor.y;
    m_data[10] = scaleFactor.z;
}

void Matrix44::setRotation(const Quaternion &q)
{
    float x2 = q.x + q.x;
    float y2 = q.y + q.y;
    float z2 = q.z + q.z;

    float xx2 = q.x * x2;
    float yy2 = q.y * y2;
    float zz2 = q.z * z2;
    float xy2 = q.x * y2;
    float xz2 = q.x * z2;
    float yz2 = q.y * z2;
    float wx2 = q.w * x2;
    float wy2 = q.w * y2;
    float wz2 = q.w * z2;

    m_data[0] = 1.0f - yy2 - zz2;
    m_data[1] = xy2 + wz2;
    m_data[2] = xz2 - wy2;
    m_data[3] = 0.0f;

    m_data[4] = xy2 - wz2;
    m_data[5] = 1.0f - xx2 - zz2;
    m_data[6] = yz2 + wx2;
    m_data[7] = 0.0f;

    m_data[8] = xz2 + wy2;
    m_data[9] = yz2 - wx2;
    m_data[10] = 1.0f - xx2 - yy2;
    m_data[11] = 0.0f;

    m_data[12] = 0.0f;
    m_data[13] = 0.0f;
    m_data[14] = 0.0f;
    m_data[15] = 1.0f;
}

void Matrix44::ortho(float left, float right, float bottom, float top, float near, float far)
{
	//m_data[0] = 2.0f/(right - left); m_data[4] = 0.0f;         m_data[8] = 0.0f;                m_data[12] = -(right + left)/(right - left);
	//m_data[1] = 0.0f;         m_data[5] = 2.0f/(top - bottom); m_data[9] = 0.0f;                m_data[13] = -(top + bottom)/(top - bottom);
	//m_data[2] = 0.0f;         m_data[6] = 0.0f;                m_data[10] = -2.0f/(far - near); m_data[14] = -(far + near)/(far - near);
	//m_data[3] = 0.0f;         m_data[7] = 0.0f;                m_data[11] = 0.0f;               m_data[15] = 1.0;

	float lr = 1 / (left - right);
	float bt = 1 / (bottom - top);
	float nf = 1 / (near - far);
    if(EngineDef::isUseVulkan)
    {
	    m_data[0] = -2 * lr;
	    m_data[1] = 0;
	    m_data[2] = 0;
	    m_data[3] = 0;

	    m_data[4] = 0;
	    m_data[5] = 2 * bt;
	    m_data[6] = 0;
	    m_data[7] = 0;


	    m_data[8] = 0;
	    m_data[9] = 0;
	    m_data[10] = 1 * nf;
	    m_data[11] = 0;

	    m_data[12] = (left + right) * lr;
	    m_data[13] = -(top + bottom) * bt;
	    m_data[14] = (near) * nf;
	    m_data[15] = 1;
    }
    else
    {
	    m_data[0] = -2 * lr;
	    m_data[1] = 0;
	    m_data[2] = 0;
	    m_data[3] = 0;
	    m_data[4] = 0;
	    m_data[5] = -2 * bt;
	    m_data[6] = 0;
	    m_data[7] = 0;
	    m_data[8] = 0;
	    m_data[9] = 0;
	    m_data[10] = 2 * nf;
	    m_data[11] = 0;
	    m_data[12] = (left + right) * lr;
	    m_data[13] = (top + bottom) * bt;
	    m_data[14] = (far + near) * nf;
	    m_data[15] = 1;
    
    }


}

#define M_PI 3.141592654
#define ToRadian(x) (float)(((x) * M_PI / 180.0f))
void Matrix44::perspective(float fovy, float aspect, float near, float far)
{
	fovy = ToRadian(fovy);
	float f = 1.0 / tan(fovy / 2);
	float nf = 1.0 / (near - far);
    if(EngineDef::isUseVulkan)
    {
	    m_data[0] = f / aspect;
	    m_data[1] = 0;
	    m_data[2] = 0;
	    m_data[3] = 0;

	    m_data[4] = 0;
	    m_data[5] = -f;
	    m_data[6] = 0;
	    m_data[7] = 0;

	    m_data[8] = 0;
	    m_data[9] = 0;
	    m_data[10] = (far) * nf;
	    m_data[11] = -1;

	    m_data[12] = 0;
	    m_data[13] = 0;
	    m_data[14] = (far * near) * nf;
	    m_data[15] = 0;
    }
    else{

	    m_data[0] = f / aspect;
	    m_data[1] = 0;
	    m_data[2] = 0;
	    m_data[3] = 0;
	    m_data[4] = 0;
	    m_data[5] = f;
	    m_data[6] = 0;
	    m_data[7] = 0;
	    m_data[8] = 0;
	    m_data[9] = 0;
	    m_data[10] = (far + near) * nf;
	    m_data[11] = -1;
	    m_data[12] = 0;
	    m_data[13] = 0;
	    m_data[14] = (2 * far * near) * nf;
	    m_data[15] = 0;
    }
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

vec4 Matrix44::transofrmVec4(vec4 v)
{
	return *this * v;
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

void Matrix44::getRowData(float* data)
{
    data[0] = m_data[0];
    data[1] = m_data[4];
    data[2] = m_data[8];
    data[3] = m_data[12];
    data[4] = m_data[1];
    data[5] = m_data[5];
    data[6] = m_data[9];
    data[7] = m_data[13];
    data[8] = m_data[2];
    data[9] = m_data[6];
    data[10] = m_data[10];
    data[11] = m_data[14];
    data[12] = m_data[3];
    data[13] = m_data[7];
    data[14] = m_data[11];
    data[15] = m_data[15];
}

void Matrix44::copyFromArray(float* data)
{
	for (int i = 0; i < 16; i++) 
	{
		m_data[i] = data[i];
	}
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

void Matrix44::stripScale()
{
	auto x = vec3(m_data[0], m_data[1], m_data[2]);
	auto y = vec3(m_data[4], m_data[5], m_data[6]);
	auto z = vec3(m_data[8], m_data[9], m_data[10]);
	x.normalize();
	y.normalize();
	z.normalize();

	m_data[0] = x.x;
	m_data[1] = x.y;
	m_data[2] = x.z;

	m_data[4] = y.x;
	m_data[5] = y.y;
	m_data[6] = y.z;

	m_data[8] = z.x;
	m_data[9] = z.y;
	m_data[10] = z.z;
}

vec3 Matrix44::getTranslation()
{
	return vec3(m_data[12], m_data[13], m_data[14]);
}

float Matrix44::determinant() const
{
	auto m = m_data;
    float a0 = m[0] * m[5] - m[1] * m[4];
    float a1 = m[0] * m[6] - m[2] * m[4];
    float a2 = m[0] * m[7] - m[3] * m[4];
    float a3 = m[1] * m[6] - m[2] * m[5];
    float a4 = m[1] * m[7] - m[3] * m[5];
    float a5 = m[2] * m[7] - m[3] * m[6];
    float b0 = m[8] * m[13] - m[9] * m[12];
    float b1 = m[8] * m[14] - m[10] * m[12];
    float b2 = m[8] * m[15] - m[11] * m[12];
    float b3 = m[9] * m[14] - m[10] * m[13];
    float b4 = m[9] * m[15] - m[11] * m[13];
    float b5 = m[10] * m[15] - m[11] * m[14];

    // Calculate the determinant.
    return (a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0);
}

bool Matrix44::decompose(vec3* scale, Quaternion* rotation, vec3* translation) const
{
	auto m = m_data;
    if (translation)
    {
        // Extract the translation.
        translation->x = m[12];
        translation->y = m[13];
        translation->z = m[14];
    }

    // Nothing left to do.
    if (scale == nullptr && rotation == nullptr)
        return true;

    // Extract the scale.
    // This is simply the length of each axis (row/column) in the matrix.
    vec3 xaxis(m[0], m[1], m[2]);
    float scaleX = xaxis.length();

    vec3 yaxis(m[4], m[5], m[6]);
    float scaleY = yaxis.length();

    vec3 zaxis(m[8], m[9], m[10]);
    float scaleZ = zaxis.length();

    // Determine if we have a negative scale (true if determinant is less than zero).
    // In this case, we simply negate a single axis of the scale.
    float det = determinant();
    if (det < 0)
        scaleZ = -scaleZ;

    if (scale)
    {
        scale->x = scaleX;
        scale->y = scaleY;
        scale->z = scaleZ;
    }

    // Nothing left to do.
    if (rotation == nullptr)
        return true;

    // Scale too close to zero, can't decompose rotation.
    if (scaleX < MATH_TOLERANCE || scaleY < MATH_TOLERANCE || std::abs(scaleZ) < MATH_TOLERANCE)
        return false;

    float rn;

    // Factor the scale out of the matrix axes.
    rn = 1.0f / scaleX;
    xaxis.x *= rn;
    xaxis.y *= rn;
    xaxis.z *= rn;

    rn = 1.0f / scaleY;
    yaxis.x *= rn;
    yaxis.y *= rn;
    yaxis.z *= rn;

    rn = 1.0f / scaleZ;
    zaxis.x *= rn;
    zaxis.y *= rn;
    zaxis.z *= rn;

    // Now calculate the rotation from the resulting matrix (axes).
    float trace = xaxis.x + yaxis.y + zaxis.z + 1.0f;

    if (trace > MATH_EPSILON)
    {
        float s = 0.5f / std::sqrt(trace);
        rotation->w = 0.25f / s;
        rotation->x = (yaxis.z - zaxis.y) * s;
        rotation->y = (zaxis.x - xaxis.z) * s;
        rotation->z = (xaxis.y - yaxis.x) * s;
    }
    else
    {
        // Note: since xaxis, yaxis, and zaxis are normalized, 
        // we will never divide by zero in the code below.
        if (xaxis.x > yaxis.y && xaxis.x > zaxis.z)
        {
            float s = 0.5f / std::sqrt(1.0f + xaxis.x - yaxis.y - zaxis.z);
            rotation->w = (yaxis.z - zaxis.y) * s;
            rotation->x = 0.25f / s;
            rotation->y = (yaxis.x + xaxis.y) * s;
            rotation->z = (zaxis.x + xaxis.z) * s;
        }
        else if (yaxis.y > zaxis.z)
        {
            float s = 0.5f / std::sqrt(1.0f + yaxis.y - xaxis.x - zaxis.z);
            rotation->w = (zaxis.x - xaxis.z) * s;
            rotation->x = (yaxis.x + xaxis.y) * s;
            rotation->y = 0.25f / s;
            rotation->z = (zaxis.y + yaxis.z) * s;
        }
        else
        {
            float s = 0.5f / std::sqrt(1.0f + zaxis.z - xaxis.x - yaxis.y);
            rotation->w = (xaxis.y - yaxis.x ) * s;
            rotation->x = (zaxis.x + xaxis.z ) * s;
            rotation->y = (zaxis.y + yaxis.z ) * s;
            rotation->z = 0.25f / s;
        }
    }

    return true;
}

bool Matrix44::getRotation(Quaternion* q)
{
	return decompose(nullptr, q, nullptr);
}

void Matrix44::getScale(vec3* scale) const
{
	decompose(scale, nullptr, nullptr);
}

} // namespace tzw
