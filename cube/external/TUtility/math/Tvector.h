#ifndef TVECTOR_H
#define TVECTOR_H

#include <stdlib.h>
namespace tzw
{
class Tvector
{
public:
    Tvector(float x =0, float y=0, float z =0);
    static float dotProduct(const Tvector & v1, const Tvector & v2);
    static Tvector reflect(Tvector in, Tvector normal);
    float x() const;
    void setX(float x);

    float y() const;
    void setY(float y);

    float z() const;
    void setZ(float z);
    const Tvector operator -(Tvector v);
    const Tvector operator * (float f)const;
    const Tvector operator *= (float f);
    const Tvector operator /(float f);
    const Tvector operator +(const Tvector v)const;
    const Tvector operator -() const;
    void negative();
    const Tvector negatived();
    float lengthSquared() const;
    static Tvector crossProduct(const Tvector & v1, const Tvector & v2);
    void normalize();
    Tvector normalized() const;
    float length() const;
private:
    float m_x,m_y,m_z;
};
}


#endif // TVECTOR_H
