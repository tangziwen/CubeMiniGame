#pragma once
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
namespace tzw
{
	class PTMILogicTickable
	{
	public:
		virtual void onDailyTick() = 0;
		virtual void onMonthlyTick() = 0;
	};

}