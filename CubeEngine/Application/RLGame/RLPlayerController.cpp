#include "RLPlayerController.h"

#include "RLHero.h"
#include "Event/EventMgr.h"
#include "RLWorld.h"
#include "Engine/Engine.h"
namespace tzw
{

	RLPlayerController::RLPlayerController()
	{
		EventMgr::shared()->addFixedPiorityListener(this);
		m_crossHairsprite = Sprite::create("RL/CrossHair.png");
		g_GetCurrScene()->addNode(m_crossHairsprite);
	}
	void RLPlayerController::possess(RLHero* hero)
	{
		m_currPossessHero = hero;

		m_currPossessHero->setIsPlayerControll(true);
		m_currPossessHero->getCollider2D()->setSourceChannel(CollisionChannel2D_Player);
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
				
				vec3 posInWorld = m_invViewMat.transformVec3(vec3(m_crossHairsprite->getPos2D().x, m_crossHairsprite->getPos2D().y, 0));
				m_currPossessHero->getWeapon()->setShootDir((posInWorld.xy() - m_currPossessHero->getPosition()).normalized());
			}
			calculateView();
			vec3 result = m_viewMat.transformVec3(vec3(0, 0, 0));
			RLWorld::shared()->getRootNode()->setPos(vec3(result.x, result.y, 0));
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
	vec2 RLPlayerController::getPos()
	{
		if(m_currPossessHero)
		{
			return m_currPossessHero->getPosition();
		}
		else
		{
			return vec2();
		}
	}
	void RLPlayerController::calculateView()
	{
		vec2 center = Engine::shared()->winSize();
		center *= 0.5f;
		float viewPosX = std::clamp(m_currPossessHero->getPosition().x - center.x, -AREAN_COLLISION_MAP_PADDING *1.f, 9999.f);
		float viewPosY = std::clamp(m_currPossessHero->getPosition().y - center.y, -AREAN_COLLISION_MAP_PADDING *1.f, 9999.f);
		m_invViewMat.setTranslate(vec3(viewPosX, viewPosY, 0));
		m_viewMat = m_invViewMat.inverted();
	}
}
