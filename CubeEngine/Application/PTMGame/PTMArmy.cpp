#include "PTMArmy.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMTile.h"
#include "PTMNation.h"
#include "2D/LabelNew.h"
#include "2D/GUIFrame.h"

#define BORDER_LEFT 0
#define BORDER_RIGHT 1
#define BORDER_BOTTOM 2
#define BORDER_TOP 3

namespace tzw
{


	PTMArmyGraphics::PTMArmyGraphics(PTMArmy* parent):
		m_parent(parent), 
		m_sprite(nullptr)
	{
		memset(m_selectedBorders, 0, sizeof(m_selectedBorders));
	}

	void PTMArmyGraphics::updateGraphics()
	{

		if(!m_button)
		{
			
			m_button = Button::create(vec4(1.0, 1.0, 1.0, 0.5), vec2(32, 32));
			m_button->setPos2D(m_parent->m_placedTile->getCanvasPos());
			m_button->setLocalPiority(2);
			m_parent->m_placedTile->m_graphics->m_sprite->getParent()->addChild(m_button);
		}
		if(!m_selectedBorders[0])
		{
			for (int i =0; i<4; i++)
			{
				m_selectedBorders[i] = GUIFrame::create(vec4(200.0/255,200.0/255,37.0/255,1.0));
				m_button->addChild(m_selectedBorders[i]);
			}
			auto contentSize = m_button->getContentSize();
			m_selectedBorders[BORDER_LEFT]->setContentSize(vec2(3,contentSize.y));
			m_selectedBorders[BORDER_RIGHT]->setContentSize(vec2(3,contentSize.y));
			m_selectedBorders[BORDER_RIGHT]->setPos2D(contentSize.x - 3,0);
			m_selectedBorders[BORDER_TOP]->setContentSize(vec2(contentSize.x,3));
			m_selectedBorders[BORDER_TOP]->setPos2D(0,contentSize.y - 3);
			m_selectedBorders[BORDER_BOTTOM]->setContentSize(vec2(contentSize.x,3));
		}
		if(!m_sprite)
		{
			m_sprite = Sprite::create("PTM/army.png");
			
			m_button->addChild(m_sprite);
		}
		if(!m_label)
		{
			m_label = LabelNew::create("5K");
			vec2 cs = m_label->getContentSize();
			m_label->setLocalPiority(1);
			m_sprite->addChild(m_label);
			m_label->setPos2D(0, - cs.y/2.0 -8);
		}

	}

	PTMArmy::PTMArmy(PTMNation * nation, PTMTile * targetTile)
		:m_parent(nation),m_placedTile(targetTile)
	{
		m_graphics = new PTMArmyGraphics(this); 
	}

	void PTMArmy::setTile(PTMTile* targetTile)
	{
		m_placedTile = targetTile;
	}

	void PTMArmy::dailyTick()
	{
	}

	void PTMArmy::updateGraphics()
	{
		m_graphics->updateGraphics();
	}


}