#pragma once
#include "Math/vec3.h"
#include <vector>
#include "GamePartType.h"
#include "3D/Thumbnail.h"
namespace tzw
{
	struct GameItem;
	struct GameItemInstance
	{
		GameItem * m_item {};
		vec3 m_tintColor;
		std::string m_surfaceName;
	};

}