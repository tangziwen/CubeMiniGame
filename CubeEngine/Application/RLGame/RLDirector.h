#pragma once
#include "Engine/EngineDef.h"
namespace tzw
{
	class RLDirector : public Singleton<RLDirector>
	{
	public:
		RLDirector();
		void tick(float dt);
	private:
		float m_time = 0.f;
	
	};
}