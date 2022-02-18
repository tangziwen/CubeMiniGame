#include "PTMInGameEvent.h"
#include "Engine/Engine.h"
#include "Engine/EngineDef.h"
#include "Utility/file/Tfile.h"
#include "2D/Sprite.h"
#include "Utility/log/Log.h"
#include "PTMNation.h"
#include "PTMInGameEvent.h"
#include "Utility/math/TbaseMath.h"
#include "PTMEventMgr.h"
namespace tzw
{

	void PTMInGameEventOption::loadFromConfig(rapidjson::Value& jsonNode)
	{
		m_Title = jsonNode["Title"].GetString();
		if(jsonNode.HasMember("Effects"))
		{
			auto & effects = jsonNode["Effects"];

			for(int i = 0; i < effects.Size(); i++)
			{
				auto &effectDoc = effects[i];
				m_effect[effectDoc["Param"].GetString()] = effectDoc["Value"].GetFloat();
			}
		}
	}

	void PTMInGameEventOption::trigger(PTMNation* nation)
	{
		for(auto &iter : m_effect)
		{
			nation->addPropByName<float>(iter.first, iter.second);
		}
		PTMEventMgr::shared()->notify(PTMEventType::PLAYER_RESOURCE_CHANGED, {});
	}

	PTMInGameEventMgr::PTMInGameEventMgr()
	{
	}
	void PTMInGameEventMgr::loadEventsFromFile(std::string filePath)
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
		auto& events = doc["Events"];
		for(auto i = 0; i < events.Size(); i++)
		{
			auto& eventDoc = events[i];
			auto inGameEvent = new PTMInGameEvent();
			inGameEvent->m_name = eventDoc["Name"].GetString();
			
			inGameEvent->m_title = eventDoc["Title"].GetString();
			inGameEvent->m_desc = eventDoc["Desc"].GetString();
			inGameEvent->m_meanTimeToHappen = eventDoc["MTTH"].GetInt();
			std::string groupName = eventDoc["Group"].GetString();
			auto iter = m_eventGroups.find(groupName);
			if(iter != m_eventGroups.end())
			{
				iter->second->m_eventsList[inGameEvent->m_name] = inGameEvent;
			}
			else
			{
				auto group = new PTMInGameEventGroup();
				group->m_eventsList[inGameEvent->m_name] = inGameEvent;
				m_eventGroups[groupName] = group;
			}
			
			//m_eventsList[inGameEvent->m_name] = inGameEvent;
			auto& options = eventDoc["Options"];
			for(auto j = 0; j < options.Size(); j++)
			{
				auto&optionDoc =options[j];
				PTMInGameEventOption option;
				option.loadFromConfig(optionDoc);
				inGameEvent->m_options.push_back(option);
			}
		}
	}
	void PTMInGameEventMgr::onMonthlyTick(PTMNation * nation)
	{
		for(auto &groupIter : m_eventGroups)
		{
			for(auto& iter : groupIter.second->m_eventsList)
			{
				float chance = 1.0f / iter.second->m_meanTimeToHappen;
				float random = TbaseMath::randRange(0.0f, 1.0f);
				if(random > (1.0f - chance))
				{
					PTMInGameEventInstanced instance;
					instance.m_parent = iter.second;
					nation->addEvent(instance);
					//break;
				}
			}
		}
	}

}
