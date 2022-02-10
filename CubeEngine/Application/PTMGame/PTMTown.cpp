#include "PTMTown.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMTile.h"
#include "PTMNation.h"
#include "2D/LabelNew.h"
namespace tzw
{


	PTMTownGraphics::PTMTownGraphics(PTMTown* parent):m_parent(parent), 
		m_townSprite(nullptr), m_flagSprite(nullptr)
	{

	}

	void PTMTownGraphics::updateGraphics()
	{
		if(!m_townSprite)
		{
			m_townSprite = Sprite::create("PTM/town.png");
			m_townSprite->setPos2D(m_parent->m_placedTile->getCanvasPos());
			m_townSprite->setLocalPiority(1);
			m_parent->m_placedTile->m_graphics->m_sprite->getParent()->addChild(m_townSprite);
			
		}
		vec4 flagColor = vec4(1.f, 0.f, 0.f, 1.0f);
		if(m_parent->m_owner)
		{
			flagColor = vec4(m_parent->m_owner->getNationColor(), 1.0f);
		}
		if(!m_flagSprite)
		{
			m_flagSprite = Sprite::createWithColor(flagColor, vec2(32, 12.f));
			m_townSprite->addChild(m_flagSprite);
		}
		else
		{
			m_flagSprite->setColor(flagColor);
		}
		if(!m_label)
		{
			m_label = LabelNew::create(m_parent->m_name);
			vec2 cs = m_label->getContentSize();
			m_label->setLocalPiority(1);
			m_townSprite->addChild(m_label);
			m_label->setPos2D(0, - cs.y/2.0 -8);
		}
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