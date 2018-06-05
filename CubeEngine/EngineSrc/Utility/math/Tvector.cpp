#include "Tvector.h"
#include <math.h>
namespace tzw
{

Tvector::Tvector(float x, float y, float z)
    :m_x(x),m_y(y),m_z(z)
{

}

float Tvector::dotProduct(const Tvector &v1, const Tvector &v2)
{
    return (v1.x() * v2.x() + v1.y() * v2.y() + v1.z() * v2.z());
}

Tvector Tvector::reflect(Tvector in, Tvector normal)
{
    return in - normal*Tvector::dotProduct (normal,in)*2;
}
float Tvector::x() const
{
    return m_x;
}

void Tvector::setX(float x)
{
    m_x = x;
}
float Tvector::y() const
{
    return m_y;
}

void Tvector::setY(float y)
{
    m_y = y;
}
float Tvector::z() const
{
    return m_z;
}

void Tvector::setZ(float z)
{
    m_z = z;
}

const Tvector Tvector::operator -(Tvector v)
{
    return Tvector(x() - v.x (), y() - v.y () , z() - v.z ());
}

const Tvector Tvector::operator *(float f) const
{
    return Tvector(x()*f, y()*f,z()*f);
}

const Tvector Tvector::operator *=(float f)
{
    ( * this) =(*this)*f ;
    return (*this);
}

const Tvector Tvector::operator /(float f)
{
    return Tvector(x()/f,y()/f,z()/f);
}

const Tvector Tvector::operator +(const Tvector v) const
{
    return Tvector(x() + v.x (),y() + v.y (), z() + v.z ());
}

const Tvector Tvector::operator -() const
{
    return Tvector(-x(),-y(),-z());
}

void Tvector::negative()
{
    m_x *= -1;
    m_y *= -1;
    m_z *= -1;
}

const Tvector Tvector::negatived()
{
    return Tvector(-x(),-y(),-z());
}

float Tvector::lengthSquared() const
{
    return x()*x() + y() * y() + z() * z();
}

Tvector Tvector::crossProduct(const Tvector &v1, const Tvector &v2)
{
    float x = (v1.y ()* v2.z ()) - (v1.z ()* v2.y ());
    float y = (v1.z ()* v2.x ()) - (v1.x ()* v2.z ());
    float z = (v1.x ()* v2.y ()) - (v1.y ()* v2.x ());
    return Tvector(x, y, z);
}

void Tvector::normalize()
{
    (*this) = this->normalized ();
}

Tvector Tvector::normalized() const
{
    Tvector a(x(),y(),z());
    a = a/ length ();
    return a;
}

float Tvector::length() const
{
    return sqrt(lengthSquared());
}

}



