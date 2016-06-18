#include "vec4.h"

namespace tzw {

vec4::vec4()
{

}

vec4::vec4(float x, float y, float z, float w)
    :x(x),y(y),z(z),w(w)
{

}

vec3 vec4::toVec3()
{
    return vec3(x,y,z);
}

} // namespace tzw

