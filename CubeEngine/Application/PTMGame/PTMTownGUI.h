#pragma once
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
#include "2D/GUIFrame.h"
#include "2D/GUITitledFrame.h"
#include "2D/Button.h"
namespace tzw
{
	class PTMTown;

	class PTMTownGUI: public Singleton<PTMTownGUI>
	{
	public:
		PTMTownGUI();
		void showInspectTown(PTMTown * newTown);
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
		PTMTown * m_inspectTown;
	};

}