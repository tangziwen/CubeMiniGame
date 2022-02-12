#include "PTMNation.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMTile.h"
#include "PTMTown.h"
#include "PTMWorld.h"
#include "PTMEventMgr.h"
namespace tzw
{

	PTMNation::PTMNation()
	{

		m_gold = 500.0;
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
	}

	void PTMNation::onMonthlyTick()
	{
		m_gold += 3;//National tax

		updateTownsMonthly();
		updateArmiesMonthly();
	}

	void PTMNation::onDailyTick()
	{
		for(PTMArmy * army : m_armyList)
		{
			army->onDailyTick();
		}
	}

	void PTMNation::updateTownsMonthly()
	{
		for(PTMTown * town: m_townList)
		{
			town->onMonthlyTick();
			m_gold += town->collectTax();
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
		m_gold += diff;
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
		m_gold -= diff;
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

}