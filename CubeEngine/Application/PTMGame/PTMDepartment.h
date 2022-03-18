#pragma once
#include "PTMBaseDef.h"
#include "PTMCurrencyEnum.h"
#include <unordered_map>
namespace tzw
{
	class PTMHero;
	class PTMNation;
	struct PTMDepartMentInputOutput
	{
		std::unordered_map<PTMCurrencyEnum, float> m_val;
		void addCurrencyAttributeType(PTMCurrencyEnum currencyType);
		void incCurrency(PTMCurrencyEnum currencyType, float val);
		void decCurrency(PTMCurrencyEnum currencyType, float val);
		void reset();
	};
	class PTMDepartment
	{

	PTM_PROPERTY_SIMPLE(BaseSlotSize, int, 0)
	PTM_PROPERTY_SIMPLE(Name, std::string, 0)
	PTM_PROPERTY_SIMPLE(HeroList, std::vector<PTMHero *>, )

	public:
		PTMDepartment(PTMNation * nation, std::string name, int baseSlotSize);
		void addHero(PTMHero * hero);
		void removeHero(PTMHero * hero);
		int getTotalSlotSize();
		virtual void calculateInputOutput();
		PTMDepartMentInputOutput * getInput();
		PTMDepartMentInputOutput * getOutput();
		virtual void work();
		PTMNation * getHomeNation();
	protected:
		PTMDepartMentInputOutput m_input;
		PTMDepartMentInputOutput m_output;
		PTMNation * m_homeNation;
	};
}