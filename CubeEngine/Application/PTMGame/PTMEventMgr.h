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
		PLAYER_DESELECT_ALL_ARMIES,
		PLAYER_SELECT_ARMY,
		CAMERA_RIGHT_CLICK_ON_TILE,
	
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
		void listen(enum class PTMEventType event_type, std::function<void(PTMEventArgPack)> function);
		void notify(enum class PTMEventType event_type, PTMEventArgPack pack);
	private:
		std::unordered_map<enum class PTMEventType, PTMEventList> m_eventList;
	};

}