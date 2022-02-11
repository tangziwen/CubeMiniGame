#pragma once
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
#include "2D/GUIFrame.h"
#include "2D/GUITitledFrame.h"
#include "2D/Button.h"
namespace tzw
{
	class PTMArmy;

	class PTMArmyGUI: public Singleton<PTMArmyGUI>
	{
	public:
		PTMArmyGUI();
		void showInspectTown(PTMArmy * newTown);
		void updateGraphics();
		void initGUI();
	private:
		GUIWindow * m_townWindow;
		LabelNew * m_name;
		LabelNew * m_ecoLevel;
		LabelNew * m_milLevel;
		Button * m_buildArmy;
		Button * m_investEco;
		Button * m_investMil;
		PTMArmy * m_inspectTown;
	};

}