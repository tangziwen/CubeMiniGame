#include "vec4.h"

namespace tzw {

vec4::vec4()
    :x(0),y(0),z(0),w(1.0)
{

}

vec4::vec4(float x, float y, float z, float w)
    :x(x),y(y),z(z),w(w)
{

}

vec4::vec4(const vec3 &v, float w)
    :x(v.x),y(v.y),z(v.z),w(w)
{

}

vec3 vec4::toVec3()
{
    return vec3(x,y,z);
}

vec4 vec4::operator +(const vec4 &other)
{
	__m128 a = _mm_load_ps(&x);
	__m128 b = _mm_load_ps(&other.x);
	__m128 c = _mm_add_ps(a, b);
	float result[4];
	_mm_store_ps(result, c);
	return vec4(result[0], result[1], result[2], result[3]);
}

tzw::vec4 vec4::operator*(float a) const
{
	return vec4(x *a, y * a, z * a, w * a);
}

vec4 vec4::operator*(vec4 a) const
{
	return vec4(x *a.x, y * a.y, z * a.z, w * a.w);
}

vec4 vec4::fromRGB(int R, int G, int B, int A)
{
	return vec4(R / 255.0, G / 255.0, B / 255.0, A / 255.0);
}

} // namespace tzw

