#pragma once
#include "Base/Singleton.h"
#include "Math/vec2.h"
#include "RLBullet.h"
#include "2D/SpriteInstanceMgr.h"
namespace tzw
{
	class RLSpritePool : public Singleton<RLSpritePool>
	{
	public:
		RLSpritePool();

		void init(Node * node);
		SpriteInstanceMgr * get();

	private:
		SpriteInstanceMgr * m_mgr;

	};

}