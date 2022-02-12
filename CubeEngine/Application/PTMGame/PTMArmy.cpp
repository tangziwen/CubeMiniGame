#include "PTMArmy.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMTile.h"
#include "PTMNation.h"
#include "2D/LabelNew.h"
#include "2D/GUIFrame.h"
#include "PTMArmyGUI.h"
#include "PTMWorld.h"
#include "PTMEventMgr.h"
#define BORDER_LEFT 0
#define BORDER_RIGHT 1
#define BORDER_BOTTOM 2
#define BORDER_TOP 3

namespace tzw
{


	PTMArmyGraphics::PTMArmyGraphics(PTMArmy* parent):
		m_parent(parent), 
		m_sprite(nullptr),
		m_label(nullptr),
		m_selectedBorders{nullptr},
		m_button(nullptr)
	{
	}

	void PTMArmyGraphics::updateGraphics()
	{
		if(!m_button)
		{
			
			m_button = Button::create(vec4(1.0, 1.0, 1.0, 0.5), vec2(32, 32));
			
			m_button->setLocalPiority(2);
			m_button->setOnBtnClicked([this](Button *)
				{

					PTMEventArgPack arg;
					arg.m_params["obj"] = m_parent;
					PTMEventMgr::shared()->notify(PTMEventType::PLAYER_SELECT_ARMY, arg);
				}
			
			);
			m_parent->m_placedTile->m_graphics->m_sprite->getParent()->addChild(m_button);
		}
		m_button->setPos2D(m_parent->m_placedTile->getCanvasPos());
		if(!m_selectedBorders[0])
		{
			for (int i =0; i<4; i++)
			{
				m_selectedBorders[i] = GUIFrame::create(vec4(200.0/255,200.0/255,37.0/255,1.0));
				m_selectedBorders[i]->setLocalPiority(100);
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
			m_label->setColor(vec4(1.0, 0.64, 0, 1));
		}
		m_label->setString(std::to_string(m_parent->m_currSize));

		for (int i =0; i<4; i++)
		{
			m_selectedBorders[i]->setIsVisible(m_parent->m_isSelected);
		}

	}

	PTMArmy::PTMArmy(PTMNation * nation, PTMTile * targetTile)
		:m_sizeLimit(1000),m_parent(nation),m_placedTile(targetTile)
	{
		m_graphics = new PTMArmyGraphics(this); 
	}

	void PTMArmy::setTile(PTMTile* targetTile)
	{
		m_placedTile = targetTile;
	}

	void PTMArmy::onMonthlyTick()
	{
		std::vector<PTMTown * >m_townList;
		for(int i = -1; i <= 1; i++)
		{
			for(int j = -1; j <= 1; j++)
			{
				
			}
		}
	}

	void PTMArmy::onDailyTick()
	{
		if(m_currSize < m_sizeLimit)
		{
			m_currSize += 50;
		}
		else
		{
			m_currSize = m_sizeLimit;//clamp
		}
		if (m_targetTile)
		{
			//fetch neibhour node
			vec2 origin = vec2(m_placedTile->coord_x, m_placedTile->coord_y);
			vec2 neighbor[4] ={vec2(origin.x + 1, origin.y), vec2(origin.x - 1, origin.y), vec2(origin.x, origin.y + 1), vec2(origin.x, origin.y - 1)};

			vec2 target(m_targetTile->coord_x, m_targetTile->coord_y);
			float minDist = 9999.f;
			int minIdx = 0;
			int idx = 0;
			for(vec2 & i : neighbor)
			{
				float halmitonDist = std::abs(target.x - i.x) + std::abs(target.y - i.y);
				if(halmitonDist < minDist)
				{
					minIdx = idx;
					minDist = halmitonDist;
				}
				++idx;
			}

			PTMTile* tile= PTMWorld::shared()->getTile((int)neighbor[minIdx].x, (int)neighbor[minIdx].y);
			m_placedTile = tile;
			if(m_placedTile == m_targetTile) //arrive
			{
				m_targetTile = nullptr;
			}
		}
		updateGraphics();
	}

	void PTMArmy::setIsSelected(bool isSelect)
	{
		m_isSelected = isSelect;
		updateGraphics();
	}

	void PTMArmy::moveTo(PTMTile* tile)
	{
		m_targetTile = tile;
	}

	void PTMArmy::updateGraphics()
	{
		m_graphics->updateGraphics();
	}


}