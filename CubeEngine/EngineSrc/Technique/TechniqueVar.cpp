#include "TechniqueVar.h"

namespace tzw {

void TechniqueVar::setT(Texture *tex)
{
    data.tex = tex;
    type = Type::Texture;
}

void TechniqueVar::setF(float value)
{
    data.f = value;
    type = Type::Float;
}

void TechniqueVar::setM(const QMatrix4x4 & value)
{
    data.m = value;
    type = Type::Matrix;
}

void TechniqueVar::setI(int value)
{
    data.i = value;
    type = Type::Integer;
}

void TechniqueVar::setV2(vec2 value)
{
    data.v2 = value;
    type = Type::Vec2;
}

void TechniqueVar::setV3(vec3 value)
{
    data.v3 = value;
    type = Type::Vec3;
}

void TechniqueVar::setV4(vec4 value)
{
    data.v4 = value;
    type = Type::Vec4;
}


} // namespace tzw

