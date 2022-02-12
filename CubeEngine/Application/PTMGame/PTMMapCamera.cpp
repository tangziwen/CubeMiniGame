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
			m_mapRootNode->setPos2D(currPos.x + m_slide * -50.f * dt, currPos.y + m_forward * -50.f * dt);
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
			vec2 mapRoot = m_mapRootNode->getWorldPos2D();
			vec2 posInMap = pos - mapRoot;
			int x = (int)posInMap.x / 32;
			int y = (int)posInMap.y / 32;
			PTMTile * tile = PTMWorld::shared()->getTile(x, y);
			//点到空的地方
			PTMEventArgPack arg;
			arg.m_params["obj"] = tile;
			PTMEventMgr::shared()->notify(PTMEventType::CAMERA_RIGHT_CLICK_ON_TILE, arg);
		}
		return false;
	}

	bool PTMMapCamera::onMousePress(int button, vec2 pos)
	{
		return false;
	}

	bool PTMMapCamera::onMouseMove(vec2 pos)
	{
		return false;
	}

	void PTMMapCamera::init()
	{
		EventMgr::shared()->addFixedPiorityListener(this);
	}

}