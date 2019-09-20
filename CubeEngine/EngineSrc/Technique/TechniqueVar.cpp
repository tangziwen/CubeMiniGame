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

void TechniqueVar::setM(const Matrix44 &value)
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

void TechniqueVar::setAsSemantic(SemanticType semanticValue)
{
	type = Type::Semantic;
	semantic = semanticValue;
}

TechniqueVar::TechniqueVar(): data()
{
	type = Type::Invalid;
	data.f_max = 999;
	data.f_min = -999;
	data.i_max = 999;
	data.i_min = 0;
	semantic = SemanticType::NO_SEMANTIC;
}

TechniqueVar *TechniqueVar::clone() const
{
    auto techVar = new TechniqueVar();
    techVar->type = type;
    techVar->data = data;
    return techVar;
}


} // namespace tzw

