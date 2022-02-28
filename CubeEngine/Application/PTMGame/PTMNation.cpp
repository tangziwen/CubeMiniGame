#include "PTMNation.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMTile.h"
#include "PTMTown.h"
#include "PTMWorld.h"
#include "PTMEventMgr.h"
#include "PTMHero.h"
#include "PTMTech.h"
#include "PTMInGameEvent.h"
namespace tzw
{
	PTMNation::PTMNation()
	{
		m_GlobalModifier = new PTMModifierContainer();
		m_Gold = 500.0;

		setPropByName<float>("AdminPoint", 500);
		setPropByName<float>("AdminPoint", 750);

		m_TechState =  PTMTechMgr::shared()->generateTechState(this, "PTM/data/Tech/AdminTech.json");
	}

	void PTMNation::millitaryOccupyTown(PTMTown* town)
	{
		m_occupyTownList.push_back(town);
		town->m_occupant = this;
	}

	void PTMNation::ownTown(PTMTown* town)
	{
		m_townList.push_back(town);
		town->m_owner = this;
		town->updateGraphics();
	}

	void PTMNation::onMonthlyTick()
	{
		m_Gold += m_NationalYearlyBaseTax / 12.0f + (*m_GlobalModifier)["national_gold_inc_modifier"];//National tax
		m_AdminPoint += m_NationalYearlyAdimPoint / 12.0f;
		m_MilitaryPoint += m_NationalYearlyMilPoint / 12.0f;
		m_GlobalManPower += 300;
		updateTownsMonthly();
		updateArmiesMonthly();

	}

	void PTMNation::onDailyTick()
	{
		for(PTMArmy * army : m_armyList)
		{
			army->onDailyTick();
		}
		for(PTMTown * town: m_townList)
		{
			town->onDailyTick();
		}
		garbageCollect();
		if(PTMWorld::shared()->getPlayerController()->getControlledNation() == this)
		{
			m_TechState->doProgress(0.1);
		}
	}

	void PTMNation::updateTownsMonthly()
	{
		for(PTMTown * town: m_townList)
		{
			town->onMonthlyTick();
			PTMTaxPack tax =  town->collectTax();
			m_Gold += tax.m_gold;
			m_AdminPoint += tax.m_adm;

		}

		if(PTMWorld::shared()->getPlayerController()->getControlledNation() == this)
		{
			PTMInGameEventMgr::shared()->onMonthlyTick(this);
		}
	}

	void PTMNation::updateArmiesMonthly()
	{
		for(PTMArmy * army: m_armyList)
		{
			army->onMonthlyTick();
		}
	}

	void PTMNation::addGold(float diff) 
	{
		m_Gold += diff;
		PTMEventArgPack pack;
		pack.m_params["object"] = this;
		if(PTMWorld::shared()->getPlayerController()->getControlledNation() == this)
		{

			PTMEventMgr::shared()->notify(PTMEventType::PLAYER_RESOURCE_CHANGED, pack);
		}
		PTMEventMgr::shared()->notify(PTMEventType::NATION_RESOURCE_CHANGED, pack);
	}

	void PTMNation::payGold(float diff)
	{
		m_Gold -= diff;
		PTMEventArgPack pack;
		pack.m_params["object"] = this;
		if(PTMWorld::shared()->getPlayerController()->getControlledNation() == this)
		{
			PTMEventMgr::shared()->notify(PTMEventType::PLAYER_RESOURCE_CHANGED, pack);
		}
		PTMEventMgr::shared()->notify(PTMEventType::NATION_RESOURCE_CHANGED, pack);
	}

	void PTMNation::addArmy(PTMArmy* army)
	{
		m_armyList.push_back(army);
	}

	void PTMNation::removeArmy(PTMArmy* army)
	{
		m_garbages.push_back(army);
	}

	std::vector<PTMTown*>& PTMNation::getTownList()
	{
		return m_townList;
	}

	void PTMNation::addEvent(const PTMInGameEventInstanced& eventInstanced)
	{
		if(m_eventTypePool.find(eventInstanced.m_parent) != m_eventTypePool.end())
			return;//no duplicate event
		m_eventTypePool[eventInstanced.m_parent] = eventInstanced;
	}

	std::unordered_map<PTMInGameEvent *, PTMInGameEventInstanced>& PTMNation::getEventInstancedList()
	{
		return m_eventTypePool;
	}

	void PTMNation::generateRandomHero()
	{
		int randomNum = rand() % 4;
		int baseNum = 6;

		for(int i = 0; i < baseNum + randomNum; i ++)
		{
			PTMHero * newHero = PTMHeroFactory::shared()->genRandomHero();
			if(!m_townList.empty())
			{
				newHero->setTownLocation(m_townList[rand() %m_townList.size()]);
			}
			
			m_heroes.push_back(newHero);
		}
	}

	PTMHero* PTMNation::getHeroAt(int index)
	{
		return m_heroes[index];
	}

	size_t PTMNation::getTotalHerosNum()
	{
		return m_heroes.size();
	}

	void PTMNation::assignTownKeeper(PTMTown* town, PTMHero* hero)
	{
		PTMHero * oldKeeper = town->getKeeper();
		if(oldKeeper) oldKeeper->setCurrRole(PTMHeroRole::Idle);
		town->setKeeper(hero);
		if(hero)
		{
			hero->setTownLocation(town);
			hero->setCurrRole(PTMHeroRole::Keeper);
		}
	}

	void PTMNation::assignOnDuty(PTMTown* town, PTMHero* hero)
	{
		town->assignOnDuty(hero);
		hero->setTownLocation(town);
		hero->setCurrRole(PTMHeroRole::OnDuty);
	}

	void PTMNation::garbageCollect()
	{
		for(PTMPawn * garbage : m_garbages)
		{
			if(garbage->getPawnType() == PawnTile::ARMY_PAWN)
			{
				auto iter = std::find(m_armyList.begin(), m_armyList.end(), garbage);
				if(iter != m_armyList.end())
				{
					m_armyList.erase(iter);
				}
				delete garbage;
			}
		}
		m_garbages.clear();
	}

}