#include "PTMFarmingDepartment.h"
#include "PTMNation.h"
namespace tzw
{
	PTMFarmingDepartment::PTMFarmingDepartment(PTMNation * nation):
		PTMDepartment(nation, "Farming", 1)
	{

	}

	void PTMFarmingDepartment::calculateInputOutput()
	{
		PTMDepartment::calculateInputOutput();
		float tmpH = 0.f;
		float tmpM = 0.f;
		auto & townList = m_homeNation->getTownList();
		for(PTMTown * t : townList)
		{
			m_output.incCurrency(PTMCurrencyEnum::Herb, 10.0f);
			m_output.incCurrency(PTMCurrencyEnum::Minerals, 15.0f);
			m_output.incCurrency(PTMCurrencyEnum::Gold, 20.0f);
		}
	}

	void PTMFarmingDepartment::work()
	{
		calculateInputOutput();
		PTMDepartment::work();
	}
}
