#include "PTMPlayerController.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMArmy.h"
#include "PTMArmyGUI.h"
#include "PTMTile.h"
#include "PTMNation.h"
#include "2D/LabelNew.h"
#include "PTMEventMgr.h"
namespace tzw
{
	PTMPlayerController::PTMPlayerController()
	{
		PTMEventMgr::shared()->listen(PTMEventType::PLAYER_DESELECT_ALL_ARMIES, 
			[this](PTMEventArgPack arg)
			{
				deSelectAll();
			}
		);

		PTMEventMgr::shared()->listen(PTMEventType::PLAYER_SELECT_ARMY, 
			[this](PTMEventArgPack arg)
			{
				PTMArmy * army = (PTMArmy *)arg.m_params["obj"];
				PTMArmyGUI::shared()->showInspectTown(army);
				selectArmy(army);
			}
		);

		PTMEventMgr::shared()->listen(PTMEventType::CAMERA_RIGHT_CLICK_ON_TILE, 
			[this](PTMEventArgPack arg)
			{
				PTMTile * tile = (PTMTile *)arg.m_params["obj"];
				if(m_selectedArmy.size() > 0)
				{
					moveSelectArmiesTo(tile);
				}
			}
		);
	}

	void PTMPlayerController::selectArmy(PTMArmy* m_army)
	{
		m_army->setIsSelected(true);
		m_selectedArmy.insert(m_army);
	}

	void PTMPlayerController::deSelect(PTMArmy* army)
	{
		auto iter = std::find(m_selectedArmy.begin(), m_selectedArmy.end(), army);
		if(iter != m_selectedArmy.end())
		{
			(*iter)->setIsSelected(false);
			m_selectedArmy.erase(iter);
		}
	}

	void PTMPlayerController::deSelectAll()
	{
		for(auto army : m_selectedArmy)
		{
			army->setIsSelected(false);
		}
		m_selectedArmy.clear();
	}

	void PTMPlayerController::moveSelectArmiesTo(PTMTile* tile)
	{
		for(auto army : m_selectedArmy)
		{
			army->moveTo(tile);
		}
	}
}
