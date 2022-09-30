#pragma once
#include "../Math/Matrix44.h"
namespace tzw
{
#pragma pack(push,1)
struct InstanceData
{
	Matrix44 transform;
	vec4 extraInfo;
	vec4 extraInfo2;
};
#pragma pack(pop)
}
