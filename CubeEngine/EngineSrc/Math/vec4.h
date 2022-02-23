#ifndef TZW_VEC4_H
#define TZW_VEC4_H
#include "vec3.h"

namespace tzw {

class alignas(16) vec4
{
public:
    vec4();
    vec4(float x,float y, float z, float w);
    vec4(const vec3 & v,float w);
    vec3 toVec3();
    float x,y,z,w;
    vec4 operator + (const vec4 & other)
    {
#ifdef TZW_USE_SIMD
	    __m128 a = _mm_load_ps(&x);
	    __m128 b = _mm_load_ps(&other.x);
	    __m128 c = _mm_add_ps(a, b);
	    float result[4];
	    _mm_store_ps(result, c);
	    return vec4(result[0], result[1], result[2], result[3]);
#else
        return vec4(x + other.x, y + other.y, z + other.z, w + other.w);
#endif
    }
	vec4 operator * (float a) const;
    vec4 operator * (vec4 a) const;
	static vec4 fromRGB(int R, int G, int B, int A = 255);
};

} // namespace tzw

#endif // TZW_VEC4_H
