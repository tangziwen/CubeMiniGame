#include "vec3.h"
#include "math.h"

namespace tzw {

vec3::vec3()
    :x(0.0f),y(0.0f),z(0.0f)
{

}

vec3::vec3(float theX):x(theX),y(theX),z(theX)
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
    __m128 a = _mm_set_ps(0.0f, z, y, x);
    __m128 b = _mm_set_ps(0.0f, other.z, other.y, other.x);
    __m128 c = _mm_add_ps(a, b);
    float result[4];
    _mm_store_ps(result, c);
    return vec3(result[0], result[1], result[2]);//vec3(x + other.x, y + other.y, z +other.z);
}

vec3 vec3::operator -(const vec3 &other) const
{
    __m128 a = _mm_set_ps(0.0f, z, y, x);
    __m128 b = _mm_set_ps(0.0f, other.z, other.y, other.x);
    __m128 c = _mm_sub_ps(a, b);
    float result[4];
    _mm_store_ps(result, c);
    return vec3(result[0], result[1], result[2]);
    //return vec3(x - other.x, y - other.y, z - other.z);
}

vec3 vec3::operator *( float val) const
{

    __m128 a = _mm_set_ps(0.0f, z, y, x);
    __m128 b = _mm_set1_ps(val);
    __m128 c = _mm_mul_ps(a, b);
    float result[4];
    _mm_store_ps(result, c);
    return vec3(result[0], result[1], result[2]);
    //auto v =vec3(x * a,y*a,z*a);
    //return v;
}

vec3 vec3::operator *(const vec3 &other) const
{
    __m128 a = _mm_set_ps(0.0f, z, y, x);
    __m128 b = _mm_set_ps(0.0f, other.z, other.y, other.x);
    __m128 c = _mm_mul_ps(a, b);
    float result[4];
    _mm_store_ps(result, c);
    return vec3(result[0], result[1], result[2]);
    //return vec3(x * other.x, y * other.y, z * other.z);
}

vec3 vec3::operator /(float val) const
{
    __m128 a = _mm_set_ps(0.0f, z, y, x);
    __m128 b = _mm_set1_ps(val);
    __m128 c = _mm_div_ps(a, b);
    float result[4];
    _mm_store_ps(result, c);
    return vec3(result[0], result[1], result[2]);
    //auto v =vec3(x / a,y / a,z / a);
    //return v;
}

vec3 vec3::operator /(const vec3 &other) const
{
    __m128 a = _mm_set_ps(0.0f, z, y, x);
    __m128 b = _mm_set_ps(0.0f, other.z, other.y, other.x);
    __m128 c = _mm_div_ps(a, b);
    float result[4];
    _mm_store_ps(result, c);
    return vec3(result[0], result[1], result[2]);
    //return vec3( x / other.x, y / other.y, z / other.z);
}

vec3 vec3::operator -() const
{
    __m128 a = _mm_set_ps(0.0f, z, y, x);
    __m128 b = _mm_setzero_ps();
    __m128 c = _mm_sub_ps(b, a);
    float result[4];
    _mm_store_ps(result, c);
    return vec3(result[0], result[1], result[2]);
    //return vec3(-x,-y,-z);
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

float vec3::getZ() const
{
    return z;
}

void vec3::setZ(float value)
{
    z = value;
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
    sprintf(tmpStr,"(%.1f, %.1f, %.1f)",x,y,z);
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

