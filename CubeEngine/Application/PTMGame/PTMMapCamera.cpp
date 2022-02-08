#include "PTMMapCamera.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
namespace tzw
{


	PTMMapCamera::PTMMapCamera(Node * mapRootNode)
		:m_mapRootNode(mapRootNode)
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
		m_mapRootNode->setPos2D(currPos.x + m_slide * -50.f * dt, currPos.y + m_forward * -50.f * dt);
	}

	void PTMMapCamera::init()
	{
		EventMgr::shared()->addFixedPiorityListener(this);
	}

}