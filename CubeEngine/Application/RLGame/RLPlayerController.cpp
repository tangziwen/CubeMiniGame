#include "RLPlayerController.h"

#include "RLHero.h"
#include "Event/EventMgr.h"
#include "RLWorld.h"
namespace tzw
{

	RLPlayerController::RLPlayerController()
	{
		EventMgr::shared()->addFixedPiorityListener(this);
	}
	void RLPlayerController::possess(RLHero* hero)
	{
		m_currPossessHero = hero;
		m_crossHairsprite = Sprite::create("RL/CrossHair.png");
		RLWorld::shared()->getRootNode()->addChild(m_crossHairsprite);
	}
	void RLPlayerController::processInput(float dt)
	{
		if(m_currPossessHero)
		{
			vec2 pos = m_currPossessHero->getPosition();
			vec2 newPos(pos.x + 50 * dt * m_xAxis, pos.y + 50 * dt * m_yAxis);
			m_currPossessHero->setPosition(newPos);
		}
	}

	void RLPlayerController::onFrameUpdate(float dt)
	{
		processInput(dt);
		if(m_currPossessHero)
		{
			if(m_currPossessHero->getWeapon())
			{
				m_currPossessHero->getWeapon()->setShootDir((m_crossHairsprite->getPos2D() - m_currPossessHero->getPosition()).normalized());
			}
		}
	}

	bool RLPlayerController::onKeyPress(int keyCode)
	{
		switch (keyCode)
		{
		case TZW_KEY_W:
			m_yAxis += 1;
			break;
		case TZW_KEY_A:
			m_xAxis -= 1;
			break;
		case TZW_KEY_S:
			m_yAxis -= 1;
			break;
		case TZW_KEY_D:
			m_xAxis += 1;
			break;
		default:
			break;
		};
		return false;
	}

	bool RLPlayerController::onKeyRelease(int keyCode)
	{
		switch (keyCode)
		{
		case TZW_KEY_W:
			m_yAxis -= 1;
			break;
		case TZW_KEY_A:
			m_xAxis += 1;
			break;
		case TZW_KEY_S:
			m_yAxis += 1;
			break;
		case TZW_KEY_D:
			m_xAxis -= 1;
			break;
		default:
			break;
		};
		return false;
	}
	bool RLPlayerController::onMouseMove(vec2 pos)
	{
		m_mousePos = pos;
		m_crossHairsprite->setPos2D(pos);
		return false;
	}
}
