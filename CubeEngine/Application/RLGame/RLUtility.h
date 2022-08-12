#pragma once
#include "Engine/EngineDef.h"
#include "Math/vec2.h"
namespace tzw
{
class RLUtility: public Singleton<RLUtility>
{
public:
	void clampToBorder(vec2 & pos);


};

}