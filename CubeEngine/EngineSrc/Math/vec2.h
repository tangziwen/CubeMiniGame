#ifndef TZW_VEC2_H
#define TZW_VEC2_H
#include <cmath>
#include <immintrin.h>

namespace tzw {

class vec2
{
public:
    vec2();
    vec2(float the_x,float the_y);
    void set(float the_x,float the_y);
    vec2 operator + (const vec2 & other) const;
    vec2 operator - (const vec2 & other) const;
    vec2 operator / (const float & other) const;
    vec2 operator * (float a) const;
    vec2& operator /= (const float & other);
    vec2& operator /= (const vec2 & other);
    vec2& operator += (const float & other);
    vec2& operator += (const vec2 & other);
    vec2& operator *= (const float & other);
    vec2& operator *= (const vec2 & other);
    vec2 normalized()
    {
#ifdef TZW_USE_SIMD
        // Must pad with a trailing 0, to store in 128-bit register
        //ALIGNED_16 platform::F32_t vector[] = {this->x, this->y, this->z, 0};
        __m128 simdvector;
        __m128 result;
        simdvector = _mm_set_ps(0, 0, y, x);

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

        return vec2(resultFloat[0], resultFloat[1]);
#else

        float len = length();
        len = 1 / len;
        return vec2(x * len, y * len);
#endif
    }
    float length() const
    {
        return sqrt(x * x + y * y );
    }
    float x,y;
    float getX() const;
    void setX(float value);
    float getY() const;
    void setY(float value);
    static float DotProduct(const vec2& left, const vec2& right)
    {
        return left.x*right.x + left.y*right.y;
    }
};

} // namespace tzw

#endif // TZW_VEC2_H
