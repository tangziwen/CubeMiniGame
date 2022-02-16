#pragma once
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
#include "PTMILogicTickable.h"
#include "PTMPawn.h"
#include "2D/GUISystem.h"
namespace tzw
{
	class LabelNew;
	struct PTMTile;
	class PTMTown;
	class PTMNation;
	class PTMArmy;
	class GUIFrame;
	class Button;

	class PTMInspectorGUI : public Singleton<PTMInspectorGUI>, public IMGUIObject
	{
	public:
		PTMInspectorGUI();
		void init();
		void drawIMGUI() override;
		void setInspectTown(PTMTown * town);
		void setInspectNation(PTMNation * nation);
		void drawNation();
	private:
		PTMTown * m_currInspectTown = nullptr;
		PTMNation * m_currInspectNation = nullptr;
	};

}