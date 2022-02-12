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
			m_parent->m_placedTile->m_graphics->m_sprite->getParent()->addChild(m_townSprite);
			m_townSprite->setTouchEnable(true);
			m_townSprite->setOnBtnClicked(
			[this](Sprite *)
			{
				PTMTownGUI::shared()->showInspectTown(m_parent);
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

	uint32_t PTMTown::getGarrisonLimit()
	{
		return m_garrisonBaseLimit + 100 * m_milDevLevel;
	}

	void PTMTown::onMonthlyTick()
	{
		m_taxAccum += m_ecoDevLevel * 0.5f + 1.0f;

		if(m_garrison < getGarrisonLimit())
		{
			m_garrison += 50 + m_milDevLevel * 20;
		}
		m_garrison =std::min(m_garrison, m_garrisonBaseLimit);//clamp
	}

	void PTMTown::onDailyTick()
	{

	}

	void PTMTown::investEco()
	{
		float totalCost = 0.0f;
		totalCost = 20.0f + m_ecoDevLevel * 15.0f;
		if (!m_owner->isCanAfford(totalCost)) return;
		m_owner->payGold(totalCost);
		m_ecoDevLevel += 1;
	}

	void PTMTown::investMil()
	{
		float totalCost = 0.0f;
		totalCost = 20.0f + m_milDevLevel * 15.0f;
		if (!m_owner->isCanAfford(totalCost)) return;
		m_owner->payGold(totalCost);
		m_milDevLevel += 1;
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

	float PTMTown::collectTax()
	{
		float tmp = m_taxAccum;
		m_taxAccum = 0.0f;
		return tmp;
	}

}