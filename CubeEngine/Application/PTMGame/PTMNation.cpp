#include "PTMNation.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMTile.h"
#include "PTMTown.h"
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
		collectTaxMonthly();
		costMonthly();
	}

	void PTMNation::collectTaxMonthly()
	{
		m_gold += 3;//National tax
		for(PTMTown * town: m_townList)
		{
			m_gold += town->getGold();
		}
	}

	void PTMNation::costMonthly()
	{
	}

	void PTMNation::addGold(float diff) 
	{
		m_gold += diff; 
	
	}

	void PTMNation::payGold(float diff)
	{
		m_gold -= diff;
	}

}