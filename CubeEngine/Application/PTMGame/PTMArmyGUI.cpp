#include "PTMArmyGUI.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMTile.h"
#include "PTMNation.h"
#include "2D/LabelNew.h"
#include "2D/Button.h"
#include <sstream>
#include <sstream>
#include "PTMPlayerController.h"
#include "PTMGameTimeMgr.h"
#include "PTMTown.h"
#include "PTMWorld.h"
#include "PTMArmy.h"
namespace tzw
{
	PTMArmyGUI::PTMArmyGUI():
		m_townWindow(nullptr),m_inspectTown(nullptr)
	{
	}

	void PTMArmyGUI::showInspectTown(PTMArmy* newTown)
	{
		m_inspectTown = newTown;
		updateGraphics();
	}

	void PTMArmyGUI::updateGraphics()
	{
		if(!m_townWindow)
		{
			initGUI();
		}
		m_townWindow->setIsVisible(true);

	}
	void PTMArmyGUI::initGUI()
	{
		m_townWindow = GUIWindow::create("Army", vec2(250, 200));


		g_GetCurrScene()->addNode(m_townWindow);
	}
}