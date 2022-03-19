#pragma once
#include <string>
#include "PTMCurrencyEnum.h"
namespace tzw
{
	struct PTMBuildingData
	{
		std::string name;
		std::string Title;
		float BuildTime;
		PTMCurrencySet BuildCost;
		float JobSize;
		PTMCurrencySet Input;
		PTMCurrencySet Output;
	};

	struct PTMBuilding
	{
		PTMBuildingData * data;
	};
}
