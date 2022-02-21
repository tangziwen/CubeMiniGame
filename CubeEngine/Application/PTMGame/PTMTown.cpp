#include "PTMTown.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMTile.h"
#include "PTMNation.h"
#include "2D/LabelNew.h"
#include "PTMTownGUI.h"
#include "PTMArmy.h"
#include "PTMWorld.h"
#include <sstream>
#include "PTMInspectorGUI.h"
namespace tzw
{


	PTMTownGraphics::PTMTownGraphics(PTMTown* parent):m_parent(parent), 
		m_townSprite(nullptr), m_flagSprite(nullptr),m_label(nullptr)
	{

	}

	void PTMTownGraphics::updateGraphics()
	{
		if(!m_townSprite)
		{
			m_townSprite = Sprite::create("PTM/town.png");
			m_townSprite->setPos2D(m_parent->m_placedTile->getCanvasPos());
			m_townSprite->setLocalPiority(1);
			PTMWorld::shared()->getMapRootNode()->addChild(m_townSprite);
			m_townSprite->setTouchEnable(true);
			m_townSprite->setOnBtnClicked(
			[this](Sprite *)
			{
				PTMInspectorGUI::shared()->setInspectTown(m_parent);
			}
			);
			
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
		if(!m_grassion)
		{
			m_grassion = LabelNew::create("100%");
			vec2 cs = m_label->getContentSize();
			m_grassion->setLocalPiority(2);
			m_townSprite->addChild(m_grassion);
		}
		std::stringstream ss;
		ss << (int)((m_parent->m_Garrison * 1.0 /  m_parent->getGarrisonLimit()) * 100.0) << "%";
		m_grassion->setString(ss.str());
	}

	PTMTown::PTMTown(PTMTile * placedTile)
		:m_occupant(nullptr),
		m_owner(nullptr)
	{
		PTMPawn::setTile(placedTile);
		m_graphics = new PTMTownGraphics(this);
	}

	void PTMTown::updateGraphics()
	{
		m_graphics->updateGraphics();
		for(PTMTile * tile : m_occupyTiles)
		{
			tile->m_graphics->updateGraphics();
		}
	}

	int PTMTown::getGarrisonLimit()
	{
		return m_GarrisonLimitBase + 100 * m_MilDevLevel;
	}

	void PTMTown::onMonthlyTick()
	{
		m_taxAccum += m_EcoDevLevel * 0.5f + 1.0f;
	}

	void PTMTown::onDailyTick()
	{
		if(m_Garrison < getGarrisonLimit())
		{
			m_Garrison += 50 + m_MilDevLevel * 20;
		}
		m_Garrison =std::min(m_Garrison, getGarrisonLimit());//clamp
		updateGraphics();
	}

	PawnTile PTMTown::getPawnType()
	{
		return PawnTile::TOWN_PAWN;
	}

	void PTMTown::investEco()
	{
		float totalCost = 0.0f;
		totalCost = 20.0f + m_EcoDevLevel * 15.0f;
		if (!m_owner->isCanAfford(totalCost)) return;
		m_owner->payGold(totalCost);
		m_EcoDevLevel += 1;
	}

	void PTMTown::investMil()
	{
		float totalCost = 0.0f;
		totalCost = 20.0f + m_MilDevLevel * 15.0f;
		if (!m_owner->isCanAfford(totalCost)) return;
		m_owner->payGold(totalCost);
		m_MilDevLevel += 1;
	}

	void PTMTown::buildArmy()
	{
		int x = m_placedTile->coord_x + 1;
		int y = m_placedTile->coord_y;
		PTMTile * targetTile = PTMWorld::shared()->getTile(x, y);

		auto army = new PTMArmy(m_owner, targetTile);
		army->updateGraphics();
		m_owner->addArmy(army);
	}

	void PTMTown::addOccupyTile(PTMTile* tile)
	{
		m_occupyTiles.push_back(tile);
		tile->m_owner = this;
	}

	float PTMTown::collectTax()
	{
		float tmp = m_taxAccum;
		m_taxAccum = 0.0f;
		return tmp;
	}

}