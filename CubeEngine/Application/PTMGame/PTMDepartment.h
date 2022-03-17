#pragma once
#include "PTMBaseDef.h"
namespace tzw
{
	class PTMHero;
	class PTMDepartment
	{

	PTM_PROPERTY_SIMPLE(BaseSlotSize, int, 0)
	PTM_PROPERTY_SIMPLE(Name, std::string, 0)
	PTM_PROPERTY_SIMPLE(HeroList, std::vector<PTMHero *>, )
	public:
		PTMDepartment(std::string name, int baseSlotSize);
		void addHero(PTMHero * hero);
		void removeHero(PTMHero * hero);
		int getTotalSlotSize();
	protected:
	
	};
}