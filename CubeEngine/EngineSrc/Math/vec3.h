#ifndef TZW_VEC3_H
#define TZW_VEC3_H
#include "Engine/EngineDef.h"
#include <string>
#include "vec2.h"
#include <immintrin.h>
namespace tzw {

class vec3
{
public:
    vec3();
	explicit vec3(float theX);
    vec3(float the_x,float the_y,float the_z);
    void set(float the_x,float the_y, float the_z);

    vec3 operator + (const vec3 & other) const
    {
#ifdef TZW_USE_SIMD
        __m128 a = _mm_set_ps(0.0f, z, y, x);
        __m128 b = _mm_set_ps(0.0f, other.z, other.y, other.x);
        __m128 c = _mm_add_ps(a, b);
        float result[4];
        _mm_store_ps(result, c);
        return vec3(result[0], result[1], result[2]);
#else
        return vec3(x + other.x, y + other.y, z +other.z);
#endif
    }

    vec3 operator + (const float other) const
    {
#ifdef TZW_USE_SIMD
        __m128 a = _mm_set_ps(0.0f, z, y, x);
        __m128 b = _mm_set_ps(0.0f, other, other, other);
        __m128 c = _mm_add_ps(a, b);
        float result[4];
        _mm_store_ps(result, c);
        return vec3(result[0], result[1], result[2]);
#else
        return vec3(x + other, y + other, z +other);
#endif
    }

    vec3 operator - (const vec3 & other) const
    {
#ifdef TZW_USE_SIMD
        __m128 a = _mm_set_ps(0.0f, z, y, x);
        __m128 b = _mm_set_ps(0.0f, other.z, other.y, other.x);
        __m128 c = _mm_sub_ps(a, b);
        float result[4];
        _mm_store_ps(result, c);
        return vec3(result[0], result[1], result[2]);
#else
        return vec3(x - other.x, y - other.y, z - other.z);
#endif
    }

    vec3 operator - (const float other) const
    {
#ifdef TZW_USE_SIMD
        __m128 a = _mm_set_ps(0.0f, z, y, x);
        __m128 b = _mm_set_ps(0.0f, other, other, other);
        __m128 c = _mm_sub_ps(a, b);
        float result[4];
        _mm_store_ps(result, c);
        return vec3(result[0], result[1], result[2]);
#else
        return vec3(x - other, y - other, z - other);
#endif
    }

    vec3 operator * (const float val) const
    {
#ifdef TZW_USE_SIMD
        __m128 a = _mm_set_ps(0.0f, z, y, x);
        __m128 b = _mm_set1_ps(val);
        __m128 c = _mm_mul_ps(a, b);
        float result[4];
        _mm_store_ps(result, c);
        return vec3(result[0], result[1], result[2]);
#else
        return vec3(x * val, y * val, z * val);
#endif

    }
    vec3 operator * (const vec3 & other) const
    {
#ifdef TZW_USE_SIMD
        __m128 a = _mm_set_ps(0.0f, z, y, x);
        __m128 b = _mm_set_ps(0.0f, other.z, other.y, other.x);
        __m128 c = _mm_mul_ps(a, b);
        float result[4];
        _mm_store_ps(result, c);
        return vec3(result[0], result[1], result[2]);
#else
        return vec3(x * other.x, y * other.y, z * other.z);
#endif
    }

    vec3 operator / (const float val) const
    {
#ifdef TZW_USE_SIMD
        __m128 a = _mm_set_ps(0.0f, z, y, x);
        __m128 b = _mm_set1_ps(val);
        __m128 c = _mm_div_ps(a, b);
        float result[4];
        _mm_store_ps(result, c);
        return vec3(result[0], result[1], result[2]);
#else
        return vec3(x / val, y / val, z / val);
#endif
    }

    vec3 operator / (const vec3 & other) const
    {
#ifdef TZW_USE_SIMD
        __m128 a = _mm_set_ps(0.0f, z, y, x);
        __m128 b = _mm_set_ps(0.0f, other.z, other.y, other.x);
        __m128 c = _mm_div_ps(a, b);
        float result[4];
        _mm_store_ps(result, c);
        return vec3(result[0], result[1], result[2]);
#else
        return vec3( x / other.x, y / other.y, z / other.z);
#endif
    }

    vec3 operator - () const
    {
#ifdef TZW_USE_SIMD
        __m128 a = _mm_set_ps(0.0f, z, y, x);
        __m128 b = _mm_setzero_ps();
        __m128 c = _mm_sub_ps(b, a);
        float result[4];
        _mm_store_ps(result, c);
        return vec3(result[0], result[1], result[2]);
#else
        return vec3(-x,-y,-z);
#endif
    
    }
    vec3& operator +=(const vec3 & other)
    {
        x+=other.x;
        y+=other.y;
        z+=other.z;
        return *this;
    }
    vec3& operator +=(const float other)
    {
#ifdef TZW_USE_SIMD
        __m128 L = _mm_set_ps(0.0f, z, y, x);
        __m128 R = _mm_set_ps(0.0f, other, other, other);

        __m128 simdvector = _mm_add_ps(L, R);
        float resultFloat[4];
        _mm_store_ps(resultFloat, simdvector);
        this->x = resultFloat[0];
        this->y = resultFloat[1];
        this->z = resultFloat[2];
#else
        x+=other;
        y+=other;
        z+=other;
#endif
        return *this;
    }
    vec3& operator -= (const vec3 & other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
    vec3& operator -= (const float & other)
    {
        x -= other;
        y -= other;
        z -= other;
        return *this;
    }

    vec3& operator *=(float value)
    {
        x*=value;
        y*=value;
        z*=value;
        return *this;
    }

    vec3& operator *=(const vec3 & other)
    {
        x*=other.x;
        y*=other.y;
        z*=other.z;
        return *this;
    }

    vec3& operator /=(float value)
    {
        x/=value;
        y/=value;
        z/=value;
        return *this;
    }

    vec3& operator /=(const vec3 & other)
    {
        x/=other.x;
        y/=other.y;
        z/=other.z;
        return *this;
    }
    static float DotProduct(const vec3& left, const vec3& right)
    {

#ifdef TZW_USE_SIMD
        __m128 x = _mm_set_ps(0.0f, left.z, left.y, left.x);
        __m128 y = _mm_set_ps(0.0f, right.z, right.y, right.x);
        __m128 mulRes, shufReg, sumsReg;
        mulRes = _mm_mul_ps(x, y);

        // Calculates the sum of SSE Register - https://stackoverflow.com/a/35270026/195787
        shufReg = _mm_movehdup_ps(mulRes);        // Broadcast elements 3,1 to 2,0
        sumsReg = _mm_add_ps(mulRes, shufReg);
        shufReg = _mm_movehl_ps(shufReg, sumsReg); // High Half -> Low Half
        sumsReg = _mm_add_ss(sumsReg, shufReg);
        float result = _mm_cvtss_f32(sumsReg); // Result in the lower part of the SSE Register
        return result;
#else
       return left.x*right.x + left.y*right.y + left.z * right.z;
#endif
    }
    static vec3 CrossProduct(const vec3& left, const vec3& right)
    {
#ifdef TZW_USE_SIMD
        __m128 x = _mm_set_ps(0.0f, left.z, left.y, left.x);
        __m128 y = _mm_set_ps(0.0f, right.z, right.y, right.x);
        __m128 tmp0 = _mm_shuffle_ps(x, x, _MM_SHUFFLE(3, 0, 2, 1));
        __m128 tmp1 = _mm_shuffle_ps(y, y, _MM_SHUFFLE(3, 1, 0, 2));
        __m128 tmp2 = _mm_shuffle_ps(x, x, _MM_SHUFFLE(3, 1, 0, 2));
        __m128 tmp3 = _mm_shuffle_ps(y, y, _MM_SHUFFLE(3, 0, 2, 1));
        __m128 resultTmp = _mm_sub_ps(_mm_mul_ps(tmp0, tmp1), _mm_mul_ps(tmp2, tmp3));
        float result[4];
        _mm_store_ps(result, resultTmp);
        return vec3(result[0], result[1], result[2]);
#else
        vec3 result;
        float x = left.x, y = left.y, z = left.z,
        x2 = right.x, y2 = right.y, z2 = right.z;
        result.x = y * z2 - z * y2;
        result.y = z * x2 - x * z2;
        result.z = x * y2 - y * x2;
        return result;
#endif
    }
    float x,y,z;
    float distance(const vec3& other)const
    {
        vec3 dir = other - (*this);
        return dir.length();
    }
    float getY() const;
    void setY(float value);
    float getX() const;
    void setX(float value);
    float getZ() const;
    void setZ(float value);
    float length() const
    {
        return sqrt(x * x + y * y + z * z);
    }
    float squaredLength() const
    {
        return x * x + y * y + z * z;
    }
    void normalize()
    {
#ifdef TZW_USE_SIMD
        // Must pad with a trailing 0, to store in 128-bit register
        //ALIGNED_16 platform::F32_t vector[] = {this->x, this->y, this->z, 0};
        __m128 simdvector;
        __m128 result;
        simdvector = _mm_set_ps(0, z, y, x);

        // (X^2, Y^2, Z^2, 0^2)
        result = _mm_mul_ps(simdvector, simdvector);

        // Add all elements together, giving us (X^2 + Y^2 + Z^2 + 0^2)
        result = _mm_hadd_ps(result, result);
        result = _mm_hadd_ps(result, result);

        // Calculate square root, giving us sqrt(X^2 + Y^2 + Z^2 + 0^2)
        result = _mm_sqrt_ps(result);

        // Calculate reciprocal, giving us 1 / sqrt(X^2 + Y^2 + Z^2 + 0^2)
        result = _mm_rcp_ps(result);

        // Finally, multiply the result with our original vector.
        simdvector = _mm_mul_ps(simdvector, result);
        float resultFloat[4];
        _mm_store_ps(resultFloat, simdvector);

        this->x = resultFloat[0];
        this->y = resultFloat[1];
        this->z = resultFloat[2];
#else
        float len = length();
        len = 1 / len;
        x*=len;
        y*=len;
        z*=len;
#endif
    }
    vec3 normalized()
    {
#ifdef TZW_USE_SIMD
        // Must pad with a trailing 0, to store in 128-bit register
        //ALIGNED_16 platform::F32_t vector[] = {this->x, this->y, this->z, 0};
        __m128 simdvector;
        __m128 result;
        simdvector = _mm_set_ps(0, z, y, x);

        // (X^2, Y^2, Z^2, 0^2)
        result = _mm_mul_ps(simdvector, simdvector);

        // Add all elements together, giving us (X^2 + Y^2 + Z^2 + 0^2)
        result = _mm_hadd_ps(result, result);
        result = _mm_hadd_ps(result, result);

        // Calculate square root, giving us sqrt(X^2 + Y^2 + Z^2 + 0^2)
        result = _mm_sqrt_ps(result);

        // Calculate reciprocal, giving us 1 / sqrt(X^2 + Y^2 + Z^2 + 0^2)
        result = _mm_rcp_ps(result);

        // Finally, multiply the result with our original vector.
        simdvector = _mm_mul_ps(simdvector, result);
        float resultFloat[4];
        _mm_store_ps(resultFloat, simdvector);

        return vec3(resultFloat[0], resultFloat[1], resultFloat[2]);
#else

        float len = length();
        len = 1 / len;
        return vec3(x * len, y * len, z * len);
#endif
    }

    vec3 scale(float scaleFactor) const
    {
#ifdef TZW_USE_SIMD
        __m128 a = _mm_set_ps(0, z, y, x);
        __m128 b = _mm_set_ps1(scaleFactor);
        __m128 c = _mm_mul_ps(a, b);
        float result[4];
        _mm_store_ps(result, c);
        return vec3(result[0], result[1], result[2]);
#else
        auto v =vec3(x * scaleFactor,y * scaleFactor,z * scaleFactor);
        return v;
#endif
    }
    void setLength(float newLength)
    {
        normalize();
        scale(newLength);
    }
    static vec3 lerp(const vec3& from, const vec3& to, float the_time)
    {

#ifdef TZW_USE_SIMD
        vec3 delta = to - from;
        float result[4];

        __m128 a = _mm_set_ps(0, the_time, the_time, the_time);
        __m128 b = _mm_set_ps(0, delta.z, delta.y, delta.x);
        __m128 c = _mm_mul_ps(a, b); // the_time * delta;
        __m128 d = _mm_set_ps(0, from.z, from.y, from.x);
        __m128 e = _mm_add_ps(d, c); // from + the_time * delta;           
        _mm_store_ps(result, e);
        return vec3(result[0], result[1], result[2]);
#else
        vec3 delta = to - from;
        vec3 result;
        result.x = from.x + the_time * delta.x;
        result.y = from.x + the_time * delta.y;
        result.z = from.z + the_time * delta.z;
        return result;
#endif
    }
	static vec3 fromRGB(int R, int G, int B);
	vec2 xy();
	vec2 xz();
	vec2 yz();
	vec3 xzy();

    std::string getStr();
};

} // namespace tzw

#endif // TZW_VEC3_H
