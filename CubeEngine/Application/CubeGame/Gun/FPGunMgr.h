#pragma once

#include "../EngineSrc/Math/vec2.h"
#include "Engine/EngineDef.h"
#include <unordered_map>

namespace tzw
{
class FPGun;
class FPGunMgr:public Singleton<FPGunMgr>
{
public:
	FPGun * loadGun(std::string filePath);
	
};

}
