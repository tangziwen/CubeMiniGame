#include "vec3.h"
#include "math.h"

namespace tzw {

vec3::vec3()
    :x(0.0f),y(0.0f),z(0.0f)
{

}

vec3::vec3(float the_x, float the_y, float the_z)
    :x(the_x),y(the_y),z(the_z)
{

}

void vec3::set(float the_x, float the_y, float the_z)
{
    x = the_x,y = the_y, z = the_z;
}

vec3 vec3::operator+(const vec3 &other) const
{
    return vec3(x + other.x, y + other.y, z +other.z);
}

vec3 vec3::operator -(const vec3 &other) const
{
    return vec3(x - other.x, y - other.y, z - other.z);
}

vec3 vec3::operator *( float a) const
{
    auto v =vec3(x * a,y*a,z*a);
    return v;
}

vec3 vec3::operator *(const vec3 &other) const
{
    return vec3(x * other.x, y * other.y, z * other.z);
}

vec3 vec3::operator /(float a) const
{
    auto v =vec3(x / a,y / a,z / a);
    return v;
}

vec3 vec3::operator /(const vec3 &other) const
{
    return vec3( x / other.x, y / other.y, z / other.z);
}

vec3 vec3::operator -() const
{
    return vec3(-x,-y,-z);
}

vec3 vec3::operator +=(const vec3 &other)
{
    x+=other.x;
    y+=other.y;
    z+=other.z;
    return vec3(x,y,z);
}

vec3 vec3::operator -=(const vec3 &other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return vec3(x,y,z);
}

vec3 vec3::operator *=(float value)
{
    x*=value;
    y*=value;
    z*=value;
    return vec3(x,y,z);
}

float vec3::DotProduct(const vec3 &left, const vec3 &right)
{
    return left.x*right.x + left.y*right.y + left.z * right.z;
}

vec3 vec3::CrossProduct(const vec3 &left, const vec3 &right)
{
    vec3 result;
    float x = left.x, y = left.y, z = left.z,
            x2 = right.x, y2 = right.y, z2 = right.z;
    result.x = y * z2 - z * y2;
    result.y = z * x2 - x * z2;
    result.z = x * y2 - y * x2;
    return result;
}

float vec3::distance(const vec3 &other) const
{
    vec3 dir = other - (*this);
    return dir.length();
}

float vec3::getZ() const
{
    return z;
}

void vec3::setZ(float value)
{
    z = value;
}

float vec3::length() const
{
    return sqrt(x * x + y * y + z * z);
}

float vec3::squaredLength() const
{
    return x * x + y * y + z * z;
}

void vec3::normalize()
{
    float len = length();
    len = 1 / len;
    x*=len;
    y*=len;
    z*=len;
}

tzw::vec3 vec3::normalized()
{
	float len = length();
	len = 1 / len;
	return vec3(x * len, y * len, z * len);
}

void vec3::setLength(float newLength)
{
    normalize();
    x *= newLength;
    y *= newLength;
    z *= newLength;
}

vec3 vec3::lerp(const vec3 &from, const vec3 &to, float the_time)
{
    vec3 delta = to - from;
    vec3 result;
    result.x = from.x + the_time*delta.x;
    result.y = from.x + the_time*delta.y;
    result.z = from.z + the_time*delta.z;
	return result;
}

vec3 vec3::fromRGB(int R, int G, int B)
{
	return vec3(R / 255.0, G / 255.0, B / 255.0);
}

vec2 vec3::xy()
{
	return vec2(x, y);
}

vec2 vec3::xz()
{
	return vec2(x, z);
}

vec2 vec3::yz()
{
	return vec2(y, z);
}

vec3 vec3::xzy()
{
	return vec3(x, z, y);
}

std::string vec3::getStr()
{
    static char tmpStr[50];
    sprintf(tmpStr,"(%f, %f, %f)",x,y,z);
    return tmpStr;
}

float vec3::getX() const
{
    return x;
}

void vec3::setX(float value)
{
    x = value;
}

float vec3::getY() const
{
    return y;
}

void vec3::setY(float value)
{
    y = value;
}

} // namespace tzw

