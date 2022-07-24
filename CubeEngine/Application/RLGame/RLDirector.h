#pragma once
#include "Engine/EngineDef.h"
namespace tzw
{
	class RLDirector : public Singleton<RLDirector>
	{
	public:
		RLDirector();
		void tick(float dt);
		void startWave();
	private:
		float m_time = 0.f;
	
	};
}