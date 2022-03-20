#include "PTMMapCamera.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMEventMgr.h"
#include "PTMWorld.h"
namespace tzw
{


	PTMMapCamera::PTMMapCamera(Node * mapRootNode)
		:m_mapRootNode(mapRootNode),m_forward(0),m_slide(0)
	{
		EventMgr::shared()->addFixedPiorityListener(this);
	}

	bool PTMMapCamera::onKeyPress(int keyCode)
	{
		switch(keyCode)
		{
		case TZW_KEY_W:
			m_forward = 1;
			break;
		case TZW_KEY_A:
			m_slide = -1;
			break;
		case TZW_KEY_S:
			m_forward = -1;
			break;
		case TZW_KEY_D:
			m_slide = 1;
			break;
		default: ;
		}
		return false;
	}

	bool PTMMapCamera::onKeyRelease(int keyCode)
	{
    switch(keyCode)
    {
    case TZW_KEY_W:
        m_forward = 0;
        break;
    case TZW_KEY_A:
        m_slide = 0;
        break;
    case TZW_KEY_S:
        m_forward = 0;
        break;
    case TZW_KEY_D:
        m_slide = 0;
        break;
	default: ;
	}
		return false;
	}

	void PTMMapCamera::onFrameUpdate(float dt)
	{
		vec2 currPos = m_mapRootNode->getPos2D();
		if(m_slide != 0 || m_forward != 0)//如果完全没变动不设置位置，避免设了个相同的位置导致大量子Node重新缓存
		{
			m_mapRootNode->setPos2D(currPos.x + m_slide * -100.f * dt, currPos.y + m_forward * -100.f * dt);
		}
	}

	bool PTMMapCamera::onMouseRelease(int button, vec2 pos)
	{
		if(button == TZW_MOUSE_BUTTON_LEFT)
		{
			//点到空的地方
			PTMEventArgPack arg;
			PTMEventMgr::shared()->notify(PTMEventType::PLAYER_DESELECT_ALL_ARMIES, arg);
		}
		if(button == TZW_MOUSE_BUTTON_RIGHT)
		{
			auto m = m_mapRootNode->getTransform();
			vec2 posInMap = m.inverted().transformVec3(vec3(pos.x, pos.y, 0)).xy();
			int x = (int)posInMap.x / 32;
			int y = (int)posInMap.y / 32;
			PTMTile * tile = PTMWorld::shared()->getTile(x, y);
			//点到空的地方
			PTMEventArgPack arg;
			arg.m_params["obj"] = tile;
			PTMEventMgr::shared()->notify(PTMEventType::CAMERA_RIGHT_CLICK_ON_TILE, arg);
		}
		if(button == TZW_MOUSE_BUTTON_MIDDLE)
		{
			m_isDragging = false;
		}
		return false;
	}

	bool PTMMapCamera::onMousePress(int button, vec2 pos)
	{
		if(button == TZW_MOUSE_BUTTON_MIDDLE)
		{
			m_isDragging = true;
			m_lastMousePos = pos;
		}
		return false;
	}

	bool PTMMapCamera::onMouseMove(vec2 pos)
	{
		if(m_isDragging)
		{
			vec2 diff = pos - m_lastMousePos;
			m_mapRootNode->setPos2D(diff + m_mapRootNode->getPos2D());
			m_lastMousePos = pos;
		}
		return false;
	}

	bool PTMMapCamera::onScroll(vec2 offset)
	{
		vec2 winSize = Engine::shared()->winSize();
		vec2 screenCenter = vec2(0.0, 0.0);
		auto vp = g_GetCurrScene()->defaultGUICamera()->getViewProjectionMatrix();
		vec4 scalingPointWorld = vp.inverted().transofrmVec4(vec4(screenCenter.x, screenCenter.y, 0.f, 1.0f));
		scalingPointWorld.x /= scalingPointWorld.w;
		scalingPointWorld.y /= scalingPointWorld.w;

		vec3 scale = m_mapRootNode->getScale();
		vec3 pos = m_mapRootNode->getPos();
		if(abs(offset.y) > 0.1)
		{
			if(offset.y > 0.5)
			{
				zoom = 1.1;
			}
			else
			{
				zoom = 0.9;
			}
		}
		Matrix44 negTransMat;
		negTransMat.setTranslate(vec3(-scalingPointWorld.x, -scalingPointWorld.y, 0.0f));
		Matrix44 scaleMat;
		scaleMat.setScale(vec3(zoom, zoom, zoom));
		Matrix44 posTransMat;
		posTransMat.setTranslate(vec3(scalingPointWorld.x, scalingPointWorld.y, 0.0f));

		Matrix44 trans;
		trans.setTranslate(vec3(pos.x, pos.y, 0.0f));

		Matrix44 finalMat = posTransMat * scaleMat * negTransMat *  m_mapRootNode->getTransform();
		vec3 newScale;
		vec3 newPos;
		finalMat.decompose(&newScale, nullptr, &newPos);
		m_mapRootNode->setScale(newScale);
		m_mapRootNode->setPos(newPos);
		printf("scroll %f %f\n",newScale.x, newScale.y);
		return false;
	}

	void PTMMapCamera::init()
	{
		EventMgr::shared()->addFixedPiorityListener(this);
	}

}