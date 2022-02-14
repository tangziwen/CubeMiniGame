#include "PTMTownGUI.h"
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
namespace tzw
{
	PTMTownGUI::PTMTownGUI():
		m_townWindow(nullptr),m_inspectTown(nullptr)
	{
	}

	void PTMTownGUI::showInspectTown(PTMTown* newTown)
	{
		m_inspectTown = newTown;
		updateGraphics();
	}

	void PTMTownGUI::updateGraphics()
	{
		if(!m_townWindow)
		{
			initGUI();
		}
		m_townWindow->setIsVisible(true);

		bool isControlledByPlayer = PTMWorld::shared()->getPlayerController()->getControlledNation() == m_inspectTown->getOwner();
		m_name->setString(m_inspectTown->getName());

		
		{
			m_buildArmy->setEnable(isControlledByPlayer);
		}

		{
			m_investEco->setEnable(isControlledByPlayer);
		}

		{
			m_investMil->setEnable(isControlledByPlayer);
		}

	}
	void PTMTownGUI::initGUI()
	{
		m_townWindow = GUIWindow::create("Town", vec2(250, 200));

		VerticalLayOut * rows = new VerticalLayOut(m_townWindow);

		HorizonalLayOut * nameRow = new HorizonalLayOut(rows);

		m_name = LabelNew::create("Name:XXXXX");
		nameRow->add(m_name);
		m_buildArmy = Button::create("Build Army");
		m_buildArmy->setOnBtnClicked([this](Button* btn){m_inspectTown->buildArmy();updateGraphics();});
		nameRow->add(m_buildArmy);



		HorizonalLayOut * ecoRow = new HorizonalLayOut(rows);
		m_ecoLevel = LabelNew::create("Eco:12345678");
		ecoRow->add(m_ecoLevel);
		m_investEco = Button::create("invest eco");
		m_investEco->setOnBtnClicked([this](Button* btn){m_inspectTown->investEco();updateGraphics();});
		ecoRow->add(m_investEco);


		HorizonalLayOut * milRow = new HorizonalLayOut(rows);
		m_milLevel = LabelNew::create("Eco:12345678");
		milRow->add(m_milLevel);
		m_investMil = Button::create("invest Mil");
		m_investMil->setOnBtnClicked([this](Button* btn){m_inspectTown->investMil();updateGraphics();});
		milRow->add(m_investMil);

		g_GetCurrScene()->addNode(m_townWindow);
	}
}