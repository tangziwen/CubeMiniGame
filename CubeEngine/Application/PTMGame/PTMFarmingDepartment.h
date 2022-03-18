#pragma once
#include "PTMDepartment.h"
namespace tzw
{
	class PTMFarmingDepartment : public PTMDepartment
	{
	public:
		PTMFarmingDepartment(PTMNation * nation);
		void calculateInputOutput() override;
		void work() override;
	};
}