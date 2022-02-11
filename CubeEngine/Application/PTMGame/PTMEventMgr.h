#pragma once
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
namespace tzw
{
	class LabelNew;
	struct PTMTile;
	class PTMTown;
	class PTMNation;
	enum class PTMEventType
	{
		NATION_RESOURCE_CHANGED,
		PLAYER_RESOURCE_CHANGED,
	
	};
	struct PTMEventArgPack
	{
		std::unordered_map<std::string, void *> m_params;
	};
	typedef std::vector<std::function<void(PTMEventArgPack)>>  PTMEventList;
	class PTMEventMgr : public Singleton<PTMEventMgr>
	{
	public:
		PTMEventMgr();
		void listen(int event_type, std::function<void(PTMEventArgPack)> function);
		void notify(int event_type, PTMEventArgPack pack);
	private:
		std::unordered_map<int, PTMEventList> m_eventList;
	};

}