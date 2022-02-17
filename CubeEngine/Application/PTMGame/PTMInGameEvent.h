#pragma once
#include "PTMPawn.h"
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
#include "PTMBaseDef.h"
namespace tzw
{
	class PTMNation;

	struct PTMInGameEventOption
	{
		std::string m_Title;
		std::unordered_map<std::string, float> m_effect;
	};

	struct PTMInGameEvent
	{
		std::string m_name;
		std::string m_title;
		std::string m_desc;
		int m_meanTimeToHappen;
		std::vector<PTMInGameEventOption> m_options;
	};

	struct PTMInGameEventInstanced
	{
		PTMInGameEvent * m_parent;
	};

	struct PTMInGameEventGroup
	{
		std::string m_group;
		std::unordered_map<std::string, PTMInGameEvent*> m_eventsList;
	};

	class PTMInGameEventMgr: public Singleton<PTMInGameEventMgr>
	{
	public:
		PTMInGameEventMgr();
		void loadEventsFromFile(std::string filePath );
		void onMonthlyTick(PTMNation * nation);
	private:
		std::unordered_map<std::string, PTMInGameEventGroup*> m_eventGroups;
		//std::unordered_map<std::string, PTMInGameEvent*> m_eventsList;
	};
}