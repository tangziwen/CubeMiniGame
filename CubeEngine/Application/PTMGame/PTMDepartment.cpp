#include "PTMDepartment.h"
#include "PTMNation.h"
#include "Utility/file/Tfile.h"
#include "Utility/log/Log.h"

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
		loadConfig();
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

		for(PTMBuilding * building  : m_buildings)
		{
			for(int i = 0; i < (int)PTMCurrencyEnum::CurrencyMax; i++)
			{
				if(building->data->Input.m_currency[i] > 0.f)
				{
					m_input.incCurrency((PTMCurrencyEnum)i, building->data->Input.m_currency[i]);
				}
				if(building->data->Output.m_currency[i] > 0.f)
				{
					m_output.incCurrency((PTMCurrencyEnum)i, building->data->Output.m_currency[i]);
				}
			}
		}
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
		if(m_currBuildingTarget)
		{
			m_buildingProgress += 30.0f;
			if(m_buildingProgress >= m_currBuildingTarget->BuildTime)
			{
				placeBuilding(m_currBuildingTarget);
			}
		}

		calculateInputOutput();
		PTMCurrencyPool * nationPool =  m_homeNation->getNationalCurrency();
		m_isWorking = true;
		for(auto & iter : m_input.m_val)
		{
			if(nationPool->isAfford(iter.first, iter.second))
			{
				nationPool->dec(iter.first, iter.second);
			}
			else
			{
				m_isWorking = false;
			}
		}
		if(m_isWorking)
		{
			for(auto & iter : m_output.m_val)
			{
				nationPool->inc(iter.first, iter.second);
			}
		}

	}

	PTMNation* PTMDepartment::getHomeNation()
	{
		return m_homeNation;
	}

	static PTMCurrencySet ParseCurrencySet(rapidjson::GenericValue<rapidjson::UTF8<>> & val)
	{
		PTMCurrencySet theSet;
		if(val.HasMember("Gold"))
		{
			theSet.m_currency[(int)PTMCurrencyEnum::Gold] = val["Gold"].GetFloat();
		}
		if(val.HasMember("Herb"))
		{
			theSet.m_currency[(int)PTMCurrencyEnum::Herb] = val["Herb"].GetFloat();
		}
		if(val.HasMember("Minerals"))
		{
			theSet.m_currency[(int)PTMCurrencyEnum::Minerals] = val["Minerals"].GetFloat();
		}
		if(val.HasMember("Dan"))
		{
			theSet.m_currency[(int)PTMCurrencyEnum::Dan] = val["Dan"].GetFloat();
		}
		return theSet;
	}

	void PTMDepartment::loadConfig()
	{
		std::string filePath = "PTM/Data/Department/";
		filePath += m_Name;
		filePath += ".json";
		rapidjson::Document doc;
		auto data = Tfile::shared()->getData(filePath, true);
		doc.Parse<rapidjson::kParseCommentsFlag | rapidjson::kParseTrailingCommasFlag>(data.getString().c_str());
		if (doc.HasParseError())
		{
			tlog("[error] get json data err! %s %d offset %d",
				filePath.c_str(),
				doc.GetParseError(),
				doc.GetErrorOffset());
			exit(1);
		}
		auto & buildings = doc["Buildings"];

		for(int i = 0; i < buildings.MemberCount(); i++)
		{
			auto& buildingDataDoc =  buildings[i];
			std::string buildingName = buildingDataDoc["Name"].GetString();
			PTMBuildingData * data = new PTMBuildingData();
			data->name = buildingName;
			data->Title = buildingDataDoc["Title"].GetString();
			data->BuildTime = buildingDataDoc["BuildTime"].GetFloat();
			data->BuildCost = ParseCurrencySet(buildingDataDoc["BuildCost"]);
			data->JobSize = buildingDataDoc["JobSize"].GetFloat();
			data->Input = ParseCurrencySet(buildingDataDoc["Input"]);
			data->Output = ParseCurrencySet(buildingDataDoc["Output"]);
			m_buildingDataList.push_back(data);
			m_buildingDataMap[buildingName] = data;
		}

		
	}

	void PTMDepartment::constructBuilding(std::string buildingName)
	{
		constructBuilding(m_buildingDataMap[buildingName]);
	}

	void PTMDepartment::constructBuilding(PTMBuildingData* buildingData)
	{
		m_currBuildingTarget = buildingData;
		m_buildingProgress = 0.f;
	}

	std::vector<PTMBuildingData*>& PTMDepartment::getBuildingDataList()
	{
		return m_buildingDataList;
	}
	std::vector<PTMBuilding*>& PTMDepartment::getBuildings()
	{
		return m_buildings;
	}
	PTMBuildingData* PTMDepartment::getCurrBuildingTarget()
	{
		return m_currBuildingTarget;
	}
	float PTMDepartment::getCurrBuildingPercentage()
	{
		return m_buildingProgress / m_currBuildingTarget->BuildTime;
	}

	bool PTMDepartment::getIsWorking()
	{
		return m_isWorking;
	}

	void PTMDepartment::placeBuilding(PTMBuildingData* buildingData)
	{
		PTMBuilding * newBuilding = new PTMBuilding();
		newBuilding->data = buildingData;
		m_buildings.push_back(newBuilding);
		m_currBuildingTarget = nullptr;
		m_buildingProgress = 0.f;
	}
}
