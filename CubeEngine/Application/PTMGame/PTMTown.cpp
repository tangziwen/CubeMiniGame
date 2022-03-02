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
#include "PTMHero.h"
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
		//m_taxAccum += m_EcoDevLevel * 0.5f + 1.0f;
	}

	void PTMTown::onDailyTick()
	{
		if(m_Garrison < getGarrisonLimit())
		{
			m_Garrison += 50 + m_MilDevLevel * 20;
		}
		m_Garrison =std::min(m_Garrison, getGarrisonLimit());//clamp
		updateGraphics();
		tickPops();
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

	void PTMTown::initPops()
	{
		int randomNum = rand() % 4;
		int baseNum = 3;
		for(int i = 0; i < baseNum + randomNum; i ++)
		{
			PTMPop newPop;
			PTMPopFactory::shared()->createAPop(&newPop);
			m_pops.push_back(newPop);
		}
	}

	PTMPop* PTMTown::getPopAt(size_t index)
	{
		return &m_pops[index];
	}

	size_t PTMTown::getTotalPopsNum()
	{
		return m_pops.size();
	}

	void PTMTown::tickPops()
	{
		tickHeroAffectOfPops();
		m_popOutputView.reset();
		for(PTMPop& pop :m_pops)
		{
			float foodProduct = pop.m_job->getFoodProduct();
			if(foodProduct> 0)
			{
				float product = foodProduct * (1.0f + m_AgriDevLevel * 0.05f + m_heroModContainer["FoodProductHeroMod"]);
				m_Food += product;
				m_popOutputView.m_FoodOutput += product;
			}
			m_Food -= pop.m_race->getFoodConsume();
			m_popOutputView.m_FoodInput += pop.m_race->getFoodConsume();
			if(m_Food < 0.f) pop.m_happiness -= 0.05f;

			float EveryDayNeedsProduct = pop.m_job->getLuxuryGoodsProduct();
			if(EveryDayNeedsProduct > 0)
			{
				float product = EveryDayNeedsProduct * (1.0f + m_EcoDevLevel * 0.05f + m_heroModContainer["EveryDayNeedsProductHeroMod"]);
				m_EveryDayNeeds += product;
				m_popOutputView.m_EveryDayNeedsOutput += product;
			}
			m_EveryDayNeeds -= pop.m_race->getEveryDayNeedsConsume();
			m_popOutputView.m_EveryDayNeedsInput += pop.m_race->getEveryDayNeedsConsume();
			
			float LuxuryGoodsProduct = pop.m_job->getLuxuryGoodsProduct();
			if(LuxuryGoodsProduct > 0)
			{
				float product = LuxuryGoodsProduct *(1.0f + m_EcoDevLevel * 0.05f + m_heroModContainer["LuxuryGoodsProductHeroMod"]);
				m_LuxuryGoods += product;
				m_popOutputView.m_LuxuryGoodsOutput += product;
			}
			m_LuxuryGoods -= pop.m_race->getLuxuryGoodsConsume();
			m_popOutputView.m_LuxuryGoodsInput += pop.m_race->getLuxuryGoodsConsume();

			//DownGrade and Upgrade pop Level, Downgrade is automaticly, upgrade is Randomness
			if(pop.m_happiness < 0.f) // Downgrade happy level
			{
				pop.m_happinessLevel = std::clamp(pop.m_happinessLevel - 1, POP_MIN_HAPPY_LEVEL, POP_MAX_HAPPY_LEVEL);
				pop.m_happiness = 1.f;
			}
			if(pop.m_happiness > 1.f) // maybe Upgrade happy level
			{
				pop.selfUpgradeMaybe();
			}

			//tax
			float goldProduct = pop.m_job->getGoldProduct();
			if(goldProduct > 0.f)
			{
				m_taxPack.m_gold += goldProduct;
			}

			float admProduct = pop.m_job->getAdmProduct();
			if(admProduct > 0.f)
			{
				m_taxPack.m_adm += admProduct;
			}

		}
		m_Food = std::clamp(m_Food, 0.0f, m_FoodCapacityBase);
		m_EveryDayNeeds = std::clamp(m_EveryDayNeeds, 0.0f, m_EveryDayNeedsCapacityBase);
		m_LuxuryGoods = std::clamp(m_LuxuryGoods, 0.0f, m_LuxuryGoodsCapacityBase);
	}


	PTMHero* PTMTown::getOnDutyHeroAt(int index)
	{
		return m_onDutyHeroes[index];
	}

	void PTMTown::assignOnDuty(PTMHero* hero)
	{
		m_onDutyHeroes.push_back(hero);
		
	}

	void PTMTown::kickOnDuty(PTMHero* hero)
	{
		auto iter = std::find(m_onDutyHeroes.begin(), m_onDutyHeroes.end(), hero);
		if(iter != m_onDutyHeroes.end())
		{
			m_onDutyHeroes.erase(iter);
		}
	}

	void PTMTown::tickHeroAffectOfPops()
	{
		m_heroesFiveElement.reset();
		m_heroModContainer.reset();
		if(m_Keeper)
		{
			m_heroesFiveElement += m_Keeper->getFiveElement() * 2.0f;
			m_Keeper->updateOutputModifier();
			m_heroModContainer.addButNoEval(m_Keeper->getOutPutModifier());
		}

		for(PTMHero * hero : m_onDutyHeroes)
		{
			m_heroesFiveElement += hero->getFiveElement() * 0.5f;
			hero->updateOutputModifier();
			m_heroModContainer.addButNoEval(hero->getOutPutModifier());
		}
		m_heroModContainer.eval();
	}

	PTMTaxPack PTMTown::collectTax()
	{
		PTMTaxPack tmp = m_taxPack;
		m_taxPack.reset();
		return tmp;
	}

}