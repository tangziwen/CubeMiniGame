#include "s_Faction.h"
namespace tzw
{
	int s_Faction::getMilitaryLevel() const
	{
		return m_militaryLevel;
	}

	void s_Faction::setMilitaryLevel(int val)
	{
		m_militaryLevel = val;
	}

	std::string s_Faction::getName() const
	{
		return m_name;
	}

	void s_Faction::setName(std::string val)
	{
		m_name = val;
	}

	int s_Faction::getGold() const
	{
		return m_gold;
	}

	void s_Faction::setGold(int val)
	{
		m_gold = val;
	}

	int s_Faction::getPoliticalLevel() const
	{
		return m_politicalLevel;
	}

	void s_Faction::setPoliticalLevel(int val)
	{
		m_politicalLevel = val;
	}

	int s_Faction::getStability() const
	{
		return m_stability;
	}

	void s_Faction::setStability(int val)
	{
		m_stability = val;
	}

}