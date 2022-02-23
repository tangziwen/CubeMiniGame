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

