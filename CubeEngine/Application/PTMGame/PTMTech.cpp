#include "PTMTech.h"
#include "Engine/Engine.h"
#include "Engine/EngineDef.h"
#include "Utility/file/Tfile.h"
#include "2D/Sprite.h"
#include "Utility/log/Log.h"
#include "PTMNation.h"
namespace tzw
{
PTMTech::PTMTech()
{
}

void PTMTech::loadFromFile(std::string filePath)
{
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
	auto& techStages = doc["TechStages"];
	for (unsigned int i = 0; i < techStages.Size(); i++)
	{
		auto& techStage = techStages[i];
		auto& techs = techStage["Techs"];
		auto stage = new PTMTechStage();
		stage->m_name = techStage["Name"].GetString();
		for (unsigned int i = 0; i < techs.Size(); i++)
		{
			auto& item = techs[i];
			PTMTechPerk perk;
			perk.m_Name = item["Name"].GetString();
			perk.m_title = item["Title"].GetString();
			auto& effects = item["Effect"];

			for (unsigned int j = 0; j < effects.Size(); j++)
			{
				auto& effect = effects[j];
				std::string paramName = effect["param"].GetString();
				perk.m_effect[paramName] = effect["value"].GetDouble();
			}
			perk.m_id = i;
			stage->m_perks.push_back(perk);
		}
		m_stages.push_back(stage);
	}

}

PTMTechStage* PTMTech::getStage(int stage)
{
	return m_stages[stage];
}

int PTMTech::getTotalStage()
{
	return m_stages.size();
}

PTMTechState::PTMTechState(PTMNation* nation, PTMTech* tech)
	:m_tech(tech), m_parent(nation)
{
	m_CurrentFocusLevel = 0;
	m_CurrentFocusIndex = 0;
	m_CurrentLevel = 0;
}

void PTMTechState::doProgress(float progressRate)
{
	//全部研发完毕
	if(m_CurrentFocusLevel >= m_tech->getTotalStage()) return;

	PTMTechStage * stage = m_tech->getStage(m_CurrentFocusLevel);
	m_techPerkMap[m_CurrentFocusLevel][m_CurrentFocusIndex] += progressRate;
	if(m_techPerkMap[m_CurrentFocusLevel][m_CurrentFocusIndex] >= 1.0)
	{
		m_techPerkMap[m_CurrentFocusLevel][m_CurrentFocusIndex] = 1.0;
		m_techPerkFinished[m_CurrentFocusLevel].insert(m_CurrentFocusIndex);

		auto& perk = stage->m_perks[m_CurrentFocusIndex];
		for(auto iter: perk.m_effect)
		{
			m_parent->addPropByName<float>(iter.first, iter.second);
		}
		//检查一下是不是整组都搞完了
		if(m_techPerkFinished[m_CurrentFocusLevel].size() == stage->m_perks.size())
		{
			m_CurrentFocusLevel += 1;
			m_CurrentFocusIndex = 0;
		}
		else //按顺序选一个新的空闲项来作为研究重心
		{
			for(auto& perk : stage->m_perks)
			{
				if(m_techPerkFinished[m_CurrentFocusLevel].find(perk.m_id) == m_techPerkFinished[m_CurrentFocusLevel].end())
				{
					m_CurrentFocusIndex = perk.m_id;
					break;
				}
			}
		}
	}
}
float PTMTechState::getProgress(int level, int perkID)
{
	return m_techPerkMap[level][perkID];
}
bool PTMTechState::isFinished(int level, int perkID)
{
	return m_techPerkFinished[level].find(perkID) != m_techPerkFinished[level].end();
}
PTMTechMgr::PTMTechMgr()
{
}
PTMTechState* PTMTechMgr::generateTechState(PTMNation * nation, std::string filePath)
{
	auto iter = m_techList.find(filePath);
	PTMTech * tech = nullptr;
	if( iter!= m_techList.end())
	{
		tech = iter->second;
	}
	else
	{
		tech = new PTMTech();
		tech->loadFromFile(filePath);
		m_techList[filePath] = tech;
	}
	PTMTechState * techState = new PTMTechState(nation, tech);
	return techState;
}
}
