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
#include "PTMDepartment.h"
#include "PTMFarmingDepartment.h"
#include "PTMAlchemyDepartment.h"
namespace tzw
{

	
	void PTMCurrencyPool::inc(PTMCurrencyEnum currencyType, float val)
	{
		m_currencyPool[(int)currencyType] += val;
	}

	void PTMCurrencyPool::dec(PTMCurrencyEnum currencyType, float val)
	{
		m_currencyPool[(int)currencyType] -= val;
	}

	float PTMCurrencyPool::get(PTMCurrencyEnum currencyType)
	{
		return m_currencyPool[(int)currencyType];
	}

	bool PTMCurrencyPool::isAfford(PTMCurrencyEnum currencyType, float costVal)
	{
		return (m_currencyPool[(int)currencyType] >= costVal);
	}


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
		for(PTMDepartment * department : m_departmentList)
		{
			department->work();
		}
		updateTownsMonthly();
		updateArmiesMonthly();

		float totalSalary = m_nationalCurrencyPool.get(PTMCurrencyEnum::Dan);
		float salary = totalSalary /  m_heroes.size();
		m_nationalCurrencyPool.dec(PTMCurrencyEnum::Dan, totalSalary);
		for(PTMHero * hero: m_heroes)
		{
			//payday mostly
			hero->payDay(salary);
			hero->onMonthlyTick();
		}
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
		for(PTMHero * hero: m_heroes)
		{
			hero->onDailyTick();
		}
		garbageCollect();
		
		/*
		if(PTMWorld::shared()->getPlayerController()->getControlledNation() == this)
		{
			m_TechState->doProgress(0.1);
		}
		*/
	}

	void PTMNation::onWeeklyTick()
	{

	}

	void PTMNation::updateTownsMonthly()
	{

		for(PTMTown * town: m_townList)
		{
			town->onMonthlyTick();
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
			newHero->setCountry(this);
			addHeroToDepartment(rand()%m_departmentList.size(), newHero);
			m_heroes.push_back(newHero);
		}
	}

	void PTMNation::initData()
	{
		m_departmentList.resize(NATION_DEPARTMENT_MAX);

		m_departmentList[NATION_DEPARTMENT_FARMING] = new PTMFarmingDepartment(this);

		m_departmentList[NATION_DEPARTMENT_ALCHEMY] = new PTMAlchemyDepartment(this);
		m_departmentList[NATION_DEPARTMENT_RESEARCH] = new PTMDepartment(this, "Research", 1);
		m_departmentList[NATION_DEPARTMENT_IDLE] = new PTMDepartment(this, "Idle", 1);

		generateRandomHero();
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
		if(oldKeeper) oldKeeper->kickFromKeeper();
		hero->assignAsKeeper(town);
	}

	void PTMNation::assignOnDuty(PTMTown* town, PTMHero* hero, int objective)
	{
		hero->assignOnDuty(town, (DutyObjectiveEnum)objective);
	}

	void PTMNation::assignResearchHero(PTMHero* hero)
	{
		hero->assignResearch();
		m_researchHeroes.push_back(hero);
	}

	void PTMNation::assignMilHero(PTMHero* hero)
	{
		hero->assignMil();
		m_MilHeroes.push_back(hero);
	}

	void PTMNation::assignAdmHero(PTMHero* hero)
	{
		hero->assignAdm();
		m_AdminHeroes.push_back(hero);
	}

	void PTMNation::assignEcoHero(PTMHero* hero)
	{
		hero->assignEco();
		m_EcoHeroes.push_back(hero);
	}

	void PTMNation::kickResearchHero(PTMHero* hero)
	{
		auto iter = std::find(m_researchHeroes.begin(), m_researchHeroes.end(), hero);
		if(iter != m_researchHeroes.end())
		{
			m_researchHeroes.erase(iter);
		}
	}

	void PTMNation::kickMilHero(PTMHero* hero)
	{
		auto iter = std::find(m_MilHeroes.begin(), m_MilHeroes.end(), hero);
		if(iter != m_MilHeroes.end())
		{
			m_MilHeroes.erase(iter);
		}
	}

	void PTMNation::kickAdmHero(PTMHero* hero)
	{
		auto iter = std::find(m_AdminHeroes.begin(), m_AdminHeroes.end(), hero);
		if(iter != m_AdminHeroes.end())
		{
			m_AdminHeroes.erase(iter);
		}
	}

	void PTMNation::kickEcoHero(PTMHero* hero)
	{
		auto iter = std::find(m_EcoHeroes.begin(), m_EcoHeroes.end(), hero);
		if(iter != m_EcoHeroes.end())
		{
			m_EcoHeroes.erase(iter);
		}
	}

	void PTMNation::addHeroToDepartment(int departmentIdx, PTMHero* hero)
	{
		if(hero->getCurrDepartment()) hero->getCurrDepartment()->removeHero(hero);
		m_departmentList[departmentIdx]->addHero(hero);
		hero->setCurrDepartment(m_departmentList[departmentIdx]);
	}

	std::vector<PTMHero*>& PTMNation::getResearchHeroes()
	{
		return m_researchHeroes;
	}

	std::vector<PTMHero*>& PTMNation::getEcoHeroes()
	{
		return m_EcoHeroes;
	}

	std::vector<PTMHero*>& PTMNation::getAdminHeroes()
	{
		return m_AdminHeroes;
	}

	std::vector<PTMHero*>& PTMNation::getMilHeroes()
	{
		return m_MilHeroes;
	}

	std::vector<PTMDepartment*>& PTMNation::getDepartments()
	{
		return m_departmentList;
	}

	PTMCurrencyPool* PTMNation::getNationalCurrency()
	{
		return &m_nationalCurrencyPool;
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