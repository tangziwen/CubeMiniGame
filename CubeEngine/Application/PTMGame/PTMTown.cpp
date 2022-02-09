#include "PTMTown.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMTile.h"
#include "PTMNation.h"
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
		if(!m_flagSprite)
		{
			vec4 flagColor = vec4(1.f, 0.f, 0.f, 1.0f);
			if(rand() % 10 > 5)
			{
				flagColor = vec4(1.f, 1.f, 0.f, 1.0f);
			}
			if(m_parent->m_owner)
			{
				flagColor = vec4(m_parent->m_owner->getNationColor(), 1.0f);
			}
			m_flagSprite = Sprite::createWithColor(flagColor, vec2(12.f, 8.f));
		}
		m_parent->m_placedTile->m_graphics->m_sprite->addChild(m_townSprite);
		m_townSprite->addChild(m_flagSprite);
	}

	PTMTown::PTMTown(PTMTile * placedTile)
		:m_placedTile(placedTile),
		m_occupant(nullptr),
		m_owner(nullptr)
	{
		m_graphics = new PTMTownGraphics(this); 
		
	}

	void PTMTown::updateGraphics()
	{
		m_graphics->updateGraphics();
	}

}