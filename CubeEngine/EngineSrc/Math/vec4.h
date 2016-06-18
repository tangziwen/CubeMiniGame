#ifndef TZW_VEC4_H
#define TZW_VEC4_H
#include "vec3.h"

namespace tzw {

class vec4
{
public:
    vec4();
    vec4(float x,float y, float z, float w);
    vec3 toVec3();
    float x,y,z,w;
};

} // namespace tzw

#endif // TZW_VEC4_H
