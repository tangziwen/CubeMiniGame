#pragma once
#include "Engine/EngineDef.h"
#include "Base/Node.h"
#include "BlockPart.h"
#include <set>
#include "Island.h"
#include "Math/Ray.h"

namespace tzw
{
struct GameItem
	{
		GameItem();

		std::string m_name;
		int m_type;
		std::string m_desc;
		std::string m_class;
	};


}
