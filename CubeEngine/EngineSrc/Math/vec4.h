#ifndef TZW_VEC4_H
#define TZW_VEC4_H
#include "vec3.h"

namespace tzw {

class vec4
{
public:
    vec4();
    vec4(float x,float y, float z, float w);
    vec4(const vec3 & v,float w);
    vec3 toVec3();
    float x,y,z,w;
    vec4 operator + (const vec4 & other);
	vec4 operator * (float a) const;
	static vec4 fromRGB(int R, int G, int B, int A = 255);
};

} // namespace tzw

#endif // TZW_VEC4_H
