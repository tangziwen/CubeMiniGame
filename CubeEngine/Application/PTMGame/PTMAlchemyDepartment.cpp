#include "PTMAlchemyDepartment.h"
#include "PTMNation.h"
namespace tzw
{
	PTMAlchemyDepartment::PTMAlchemyDepartment(PTMNation * nation):
		PTMDepartment(nation, "Alchemy", 1)
	{

	}

	void PTMAlchemyDepartment::calculateInputOutput()
	{
		PTMDepartment::calculateInputOutput();
		float tmpH = 0.f;
		float tmpM = 0.f;
		auto & townList = m_homeNation->getTownList();
		m_input.incCurrency(PTMCurrencyEnum::Herb, 0.5f);
		for(PTMTown * t : townList)
		{
			m_output.incCurrency(PTMCurrencyEnum::Dan, 10.0f);
		}
	}

	void PTMAlchemyDepartment::work()
	{
		calculateInputOutput();
		PTMDepartment::work();
	}
}
