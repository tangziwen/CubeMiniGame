#include "Quaternion.h"
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include "Utility/math/TbaseMath.h"
#include "Matrix44.h"
namespace tzw {

Quaternion::Quaternion()
{

}

Quaternion::Quaternion(float theX, float theY, float theZ, float theW)
{
    x = theX;
    y = theY;
    z = theZ;
    w = theW;
}

void Quaternion::fromRotationMatrix(Matrix44 *mat)
{
        // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
        // article "Quaternion Calculus and Fast Animation".
		float (*kRot)[4] = (float(*)[4])mat->data();
        float fTrace = kRot[0][0]+kRot[1][1]+kRot[2][2];
        float fRoot;

        if ( fTrace > 0.0 )
        {
            // |w| > 1/2, may as well choose w > 1/2
            fRoot = sqrt(fTrace + 1.0f);  // 2w
            w = 0.5f*fRoot;
            fRoot = 0.5f/fRoot;  // 1/(4w)
            x = (kRot[2][1]-kRot[1][2])*fRoot;
            y = (kRot[0][2]-kRot[2][0])*fRoot;
            z = (kRot[1][0]-kRot[0][1])*fRoot;
        }
        else
        {
            // |w| <= 1/2
            static size_t s_iNext[3] = { 1, 2, 0 };
            size_t i = 0;
            if ( kRot[1][1] > kRot[0][0] )
                i = 1;
            if ( kRot[2][2] > kRot[i][i] )
                i = 2;
            size_t j = s_iNext[i];
            size_t k = s_iNext[j];

            fRoot = sqrt(kRot[i][i]-kRot[j][j]-kRot[k][k] + 1.0f);
            float* apkQuat[3] = { &x, &y, &z };
            *apkQuat[i] = 0.5f*fRoot;
            fRoot = 0.5f/fRoot;
            w = (kRot[k][j]-kRot[j][k])*fRoot;
            *apkQuat[j] = (kRot[j][i]+kRot[i][j])*fRoot;
            *apkQuat[k] = (kRot[k][i]+kRot[i][k])*fRoot;
        }
}



void Quaternion::fromEulerAngle(vec3 rotate)
{
    float halfRadx = TbaseMath::Ang2Radius(rotate.x / 2.f),
            halfRady = TbaseMath::Ang2Radius(rotate.y / 2.f),
            halfRadz = -TbaseMath::Ang2Radius(rotate.z / 2.f);
    float coshalfRadx = cosf(halfRadx), sinhalfRadx = sinf(halfRadx), coshalfRady = cosf(halfRady), sinhalfRady = sinf(halfRady), coshalfRadz = cosf(halfRadz), sinhalfRadz = sinf(halfRadz);
    x = sinhalfRadx * coshalfRady * coshalfRadz - coshalfRadx * sinhalfRady * sinhalfRadz;
    y = coshalfRadx * sinhalfRady * coshalfRadz + sinhalfRadx * coshalfRady * sinhalfRadz;
    z = coshalfRadx * coshalfRady * sinhalfRadz - sinhalfRadx * sinhalfRady * coshalfRadz;
    w = coshalfRadx * coshalfRady * coshalfRadz + sinhalfRadx * sinhalfRady * sinhalfRadz;

}

void Quaternion::fromAxisAngle(vec3 axis, float angle)
{
    float half = TbaseMath::Ang2Radius(angle / 2.f);
    float sinHalf = sinf(half);
    w = cosf(half);
    x = sinHalf * cosf(axis.x);
    y = sinHalf * cosf(axis.y);
    z = sinHalf * cosf(axis.z);
}

void Quaternion::toEulserAngel(float *resultX, float *resultY, float *resultZ) const
{
    float _rotationX,_rotationY,_rotationZ;


    //convert quaternion to Euler angle
    double test = x * y + z * w;
    if (test > 0.499)
    {
        _rotationX = 3.14/2.0f;
        _rotationY = 2 * atan2(x,w);
        _rotationZ = 0;
    }
    else if (test < -0.499)
    {
        _rotationX = - 3.14/2.0f;
        _rotationY = - 2 * atan2(x,w);
        _rotationZ = 0;
    }
    else
    {
        _rotationX = atan2f(2.f * (w * x + y * z), 1.f - 2.f * (x * x + y * y));
        float sy = 2.f * (w * y - z * x);
        sy = TbaseMath::clampf(sy, -1, 1);
        _rotationY = asinf(sy);
        _rotationZ = atan2f(2.f * (w * z + x * y), 1.f - 2.f * (y * y + z * z));
    }
    _rotationX = TbaseMath::Radius2Ang(_rotationX);
    _rotationY = TbaseMath::Radius2Ang(_rotationY);
    _rotationZ = -TbaseMath::Radius2Ang(_rotationZ);

    (*resultX) = _rotationX;
    (*resultY) = _rotationY;
    (*resultZ) = _rotationZ;
}

void Quaternion::fromAxises(vec3 xaxis, vec3 yaxis, vec3 zaxis)
{
    float trace = xaxis.x + yaxis.y + zaxis.z + 1.0f;

    if (trace > 0.01)
    {
        float s = 0.5f / sqrtf(trace);
        w = 0.25f / s;
        x = (yaxis.z - zaxis.y) * s;
        y = (zaxis.x - xaxis.z) * s;
        z = (xaxis.y - yaxis.x) * s;
    }
    else
    {
        // Note: since xaxis, yaxis, and zaxis are normalized,
        // we will never divide by zero in the code below.
        if (xaxis.x > yaxis.y && xaxis.x > zaxis.z)
        {
            float s = 0.5f / sqrtf(1.0f + xaxis.x - yaxis.y - zaxis.z);
            w = (yaxis.z - zaxis.y) * s;
            x = 0.25f / s;
            y = (yaxis.x + xaxis.y) * s;
            z = (zaxis.x + xaxis.z) * s;
        }
        else if (yaxis.y > zaxis.z)
        {
            float s = 0.5f / sqrtf(1.0f + yaxis.y - xaxis.x - zaxis.z);
            w = (zaxis.x - xaxis.z) * s;
            x = (yaxis.x + xaxis.y) * s;
            y = 0.25f / s;
            z = (zaxis.y + yaxis.z) * s;
        }
        else
        {
            float s = 0.5f / sqrtf(1.0f + zaxis.z - xaxis.x - yaxis.y );
            w = (xaxis.y - yaxis.x ) * s;
            x = (zaxis.x + xaxis.z ) * s;
            y = (zaxis.y + yaxis.z ) * s;
            z = 0.25f / s;
        }
    }
}

void Quaternion::normalize()
{
    float len = sqrtf(x * x + y * y + z * z + w * w);
    if (fabs(len) <= 0.0000001) {
        x = 0;
        y = 0;
        z = 0;
        w = 0;
        return ;
    }
    len = 1 / len;
    x = x * len;
    y = y * len;
    z = z * len;
    w = w * len;
}

Quaternion Quaternion::slerp(Quaternion start, Quaternion end, float dt)
{
    Quaternion result;
    float cosHalfTheta = start[0] * end[0] + start[1] * end[1] + start[2] * end[2] + start[3] * end[3],
            halfTheta,
            sinHalfTheta,
            ratioA,
            ratioB;

    if (fabsf(cosHalfTheta) >= 1.0) {
        result = start;
        return result;
    }

    halfTheta = acosf(cosHalfTheta);
    sinHalfTheta = sqrtf(1.0f - cosHalfTheta * cosHalfTheta);

    if (fabsf(sinHalfTheta) < 0.001f) {
        result[0] = (start[0] * 0.5f + end[0] * 0.5f);
        result[1] = (start[1] * 0.5f + end[1] * 0.5f);
        result[2] = (start[2] * 0.5f + end[2] * 0.5f);
        result[3] = (start[3] * 0.5f + end[3] * 0.5f);
        return result;
    }

    ratioA = sinf((1 - dt) * halfTheta) / sinHalfTheta;
    ratioB = sinf(dt * halfTheta) / sinHalfTheta;

    result[0] = (start[0] * ratioA + end[0] * ratioB);
    result[1] = (start[1] * ratioA + end[1] * ratioB);
    result[2] = (start[2] * ratioA + end[2] * ratioB);
    result[3] = (start[3] * ratioA + end[3] * ratioB);
    return result;
}

Quaternion Quaternion::lerp(Quaternion start, Quaternion end, float dt)
{
    Quaternion result;
    float t1 = 1.0f - dt;

    for(int i = 0;i<=3;i++)
    {
        result[i] = t1 * start[i] + dt * end[i];
    }
    return result;
}

float &Quaternion::operator [](int index)
{
    switch(index)
    {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    case 3:
        return w;
    default:
        return x;
    }
}

Quaternion Quaternion::operator *(const Quaternion &other)
{
    Quaternion q1 = (*this);
    Quaternion q2 = other;
    Quaternion result;
    result.x =  q1.x * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x;
    result.y = -q1.x * q2.z + q1.y * q2.w + q1.z * q2.x + q1.w * q2.y;
    result.z =  q1.x * q2.y - q1.y * q2.x + q1.z * q2.w + q1.w * q2.z;
    result.w = -q1.x * q2.x - q1.y * q2.y - q1.z * q2.z + q1.w * q2.w;
    return result;
}

} // namespace tzw
