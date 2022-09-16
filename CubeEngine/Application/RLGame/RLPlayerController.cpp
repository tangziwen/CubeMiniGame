#include "RLPlayerController.h"

#include "RLHero.h"
#include "Event/EventMgr.h"
#include "RLWorld.h"
#include "Engine/Engine.h"
#include "RLBullet.h"
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
		m_currPossessHero->setController(this);
		m_currPossessHero->setIsPlayerControll(true);
		m_currPossessHero->onPossessed();

	}
	void RLPlayerController::processInput(float dt)
	{
		if(m_currPossessHero)
		{
			vec2 pos = m_currPossessHero->getPosition();
			
			m_currPossessHero->m_speed = 120.0f;
			if(fabs(m_xAxis) > 0 || fabs(m_yAxis) > 0)
			{
				vec2 moveDir(m_xAxis, m_yAxis);
				moveDir = moveDir.normalized();
				//vec2 newPos(pos.x + m_currPossessHero->m_speed * dt * moveDir.x, pos.y + m_currPossessHero->m_speed * dt * moveDir.y);
				//m_currPossessHero->setPosition(newPos);

				m_currPossessHero->doMove(moveDir, dt);
			}
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
		case TZW_KEY_SPACE:
			m_currPossessHero->doDash();
			break;
		case TZW_KEY_ESCAPE:
			RLWorld::shared()->goToPause();
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
	bool RLPlayerController::onMouseRelease(int button, vec2 pos)
	{
		if(button == TZW_MOUSE_BUTTON_LEFT)
		{
			m_currPossessHero->getWeapon()->setIsAutoFiring(false);

		}
		if(button == TZW_MOUSE_BUTTON_RIGHT)
		{
			m_currPossessHero->endDeflect();
		}
		return false;
	}
	bool RLPlayerController::onMousePress(int button, vec2 pos)
	{
		if(button == TZW_MOUSE_BUTTON_LEFT)
		{
			m_currPossessHero->getWeapon()->setIsAutoFiring(true);

		}
		if(button == TZW_MOUSE_BUTTON_RIGHT)
		{
			m_currPossessHero->startDeflect();


		}
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
#pragma optimize("", off)
	void RLPlayerController::calculateView()
	{
		vec2 center = Engine::shared()->winSize();
		center *= 0.5f;
		float p_x = m_currPossessHero->getPosition().x;
		float p_y = m_currPossessHero->getPosition().y;
		float viewPosX = std::clamp(p_x - center.x, -AREAN_COLLISION_MAP_PADDING *1.f, 9999.f);
		float viewPosY = std::clamp(p_y - center.y, -AREAN_COLLISION_MAP_PADDING *1.f, 9999.f);
		m_invViewMat.setTranslate(vec3(viewPosX, viewPosY, 0));
		m_viewMat = m_invViewMat.inverted();
	}
}
