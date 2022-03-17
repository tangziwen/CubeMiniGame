#include "PTMDepartment.h"
namespace tzw
{

	PTMDepartment::PTMDepartment(std::string name, int baseSlotSize)
	{
		m_Name = name;
		m_BaseSlotSize = baseSlotSize;
	}
	void PTMDepartment::addHero(PTMHero* hero)
	{
		m_HeroList.push_back(hero);
	}
	void PTMDepartment::removeHero(PTMHero* hero)
	{
		auto iter = std::find(m_HeroList.begin(), m_HeroList.end(), hero);
		if(iter != m_HeroList.end())
		{
			m_HeroList.erase(iter);
		}
	}
	int PTMDepartment::getTotalSlotSize()
	{
		return m_BaseSlotSize;
	}
}
