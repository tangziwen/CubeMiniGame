#ifndef TZW_QUATERNION_H
#define TZW_QUATERNION_H

#include "vec3.h"
namespace tzw {

class Quaternion
{
public:
    Quaternion();
    Quaternion(float theX,float theY,float theZ,float theW);
    void fromEulerAngle(vec3 rotate);
    void toEulserAngel(float * resultX, float * resultY, float * resultZ) const;
    void fromAxises(vec3 X_Axis, vec3 Y_Axis, vec3 zaxis);
    void normalize();
    static Quaternion slerp(Quaternion start,Quaternion end,float dt);
    static Quaternion  lerp(Quaternion start,Quaternion end,float dt);
    float& operator [](int index);
    Quaternion operator * (const Quaternion & other);
    float x,y,z,w;
};

} // namespace tzw

#endif // TZW_QUATERNION_H
