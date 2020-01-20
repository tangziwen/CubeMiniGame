#include "TechniqueVar.h"

namespace tzw {
	TechniqueVar::VarData::RawData::RawData()
	{
	}

void TechniqueVar::setT(Texture *tex, int slotID)
{
    data.rawData.texInfo.tex = tex;
	data.rawData.texInfo.slotID = slotID;
    type = Type::Texture;
}

void TechniqueVar::setF(float value)
{
    data.rawData.f = value;
    type = Type::Float;
}

void TechniqueVar::setM(const Matrix44 &value)
{
    data.rawData.m = value;
    type = Type::Matrix;
}

void TechniqueVar::setI(int value)
{
    data.rawData.i = value;
    type = Type::Integer;
}

void TechniqueVar::setV2(vec2 value)
{
    data.rawData.v2 = value;
    type = Type::Vec2;
}

void TechniqueVar::setV3(vec3 value)
{
    data.rawData.v3 = value;
    type = Type::Vec3;
}

void TechniqueVar::setV4(vec4 value)
{
    data.rawData.v4 = value;
    type = Type::Vec4;
}

void TechniqueVar::setAsSemantic(SemanticType semanticValue)
{
	type = Type::Semantic;
	semantic = semanticValue;
}

TechniqueVar::TechniqueVar()
{
	type = Type::Invalid;
	data.f_max = 999;
	data.f_min = -999;
	data.i_max = 999;
	data.i_min = 0;
	//data.rawData.tex = nullptr;
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

