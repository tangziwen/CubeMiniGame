#pragma once
#include "PTMDepartment.h"
namespace tzw
{
	class PTMAlchemyDepartment : public PTMDepartment
	{
	public:
		PTMAlchemyDepartment(PTMNation * nation);
		void calculateInputOutput() override;
		void work() override;
	};
}