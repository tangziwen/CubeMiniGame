#include "PTMEventMgr.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMTile.h"
#include "PTMNation.h"
#include "2D/LabelNew.h"
namespace tzw
{




	PTMEventMgr::PTMEventMgr()
	{
	}

	void PTMEventMgr::listen(enum class PTMEventType event_type, std::function<void(PTMEventArgPack)> fuction)
	{
		auto iter = m_eventList.find(event_type);
		if(iter == m_eventList.end())
		{
			PTMEventList eventList;
			m_eventList[event_type] = eventList;
		}
		m_eventList[event_type].push_back(fuction);
	}

	void PTMEventMgr::notify(enum class PTMEventType event_type, PTMEventArgPack pack)
	{
		auto iter = m_eventList.find(event_type);
		if(iter == m_eventList.end())
		{
			return;
		}

		for(auto cb : iter->second)
		{
			cb(pack);
		}
	}

}