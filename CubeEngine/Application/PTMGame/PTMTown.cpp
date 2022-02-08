#include "PTMTown.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMTile.h"
namespace tzw
{


	PTMTownGraphics::PTMTownGraphics(PTMTown* parent):m_parent(parent), 
		m_townSprite(nullptr)
	{

	}

	void PTMTownGraphics::updateGraphics()
	{
		if(!m_townSprite)
		{
			m_townSprite = Sprite::create("PTM/town.png");
		}
		m_parent->m_placedTile->m_graphics->m_sprite->addChild(m_townSprite);
	}

	PTMTown::PTMTown(PTMTile * placedTile)
		:m_placedTile(placedTile)
	{
		m_graphics = new PTMTownGraphics(this); 
		
	}

	void PTMTown::updateGraphics()
	{
		m_graphics->updateGraphics();
	}

}