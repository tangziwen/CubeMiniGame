#include "PTMDepartment.h"
#include "PTMNation.h"
namespace tzw
{

	void PTMDepartMentInputOutput::addCurrencyAttributeType(PTMCurrencyEnum currencyType)
	{
		m_val[currencyType] = 0.f;
	}

	void PTMDepartMentInputOutput::incCurrency(PTMCurrencyEnum currencyType, float val)
	{
		auto iter = m_val.find(currencyType);
		if(iter != m_val.end())
		{
			iter->second += val;
		}
		else
		{
			m_val[currencyType] = val;
		}
	}

	void PTMDepartMentInputOutput::decCurrency(PTMCurrencyEnum currencyType, float val)
	{
		auto iter = m_val.find(currencyType);
		if(iter != m_val.end())
		{
			iter->second -= val;
		}
		else
		{
			m_val[currencyType] = -val;
		}
	}

	void PTMDepartMentInputOutput::reset()
	{
		m_val.clear();
	}

	PTMDepartment::PTMDepartment(PTMNation * nation, std::string name, int baseSlotSize)
	{
		m_Name = name;
		m_BaseSlotSize = baseSlotSize;
		m_homeNation = nation;
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

	void PTMDepartment::calculateInputOutput()
	{
		m_input.reset();
		m_output.reset();
	}

	PTMDepartMentInputOutput* PTMDepartment::getInput()
	{
		return &m_input;
	}
	PTMDepartMentInputOutput* PTMDepartment::getOutput()
	{
		return &m_output;
	}

	void PTMDepartment::work()
	{
		calculateInputOutput();
		PTMCurrencyPool * nationPool =  m_homeNation->getNationalCurrency();
		for(auto & iter : m_input.m_val)
		{
			nationPool->dec(iter.first, iter.second);
		}
		for(auto & iter : m_output.m_val)
		{
			nationPool->inc(iter.first, iter.second);
		}
	}

	PTMNation* PTMDepartment::getHomeNation()
	{
		return m_homeNation;
	}

}
