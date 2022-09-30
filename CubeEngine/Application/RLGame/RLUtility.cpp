#include "RLUtility.h"
#include "RLDef.h"
#include <algorithm>
namespace tzw
{
	void RLUtility::clampToBorder(vec2& pos)
	{
		pos.x = std::clamp(pos.x, 16.f, ARENA_MAP_SIZE *32.f - 16.f);
		pos.y = std::clamp(pos.y, 16.f, ARENA_MAP_SIZE *32.f - 16.f);
	}


}