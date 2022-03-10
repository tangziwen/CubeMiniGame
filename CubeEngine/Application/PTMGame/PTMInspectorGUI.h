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
	class PTMHero;
	class PTMArmy;

	class PTMInspectorGUI : public Singleton<PTMInspectorGUI>, public IMGUIObject
	{
	public:
		PTMInspectorGUI();
		void init();
		void drawIMGUI() override;
		void setInspectTown(PTMTown * town);
		void setInspectNation(PTMNation * nation);
		void setInspectTownList(PTMNation * nation, std::function<void(PTMTown*)> clickCB);
		void setInspectHeroList(PTMNation * nation, std::function<void(PTMHero*)> clickCB);
		void setInspectHero(PTMHero * hero);
		void setInspectArmy(PTMArmy * army);
		void drawNation();
		void drawEvents();
		void drawHeroSmall(PTMHero * hero, bool isShowLocation);
		void drawHeroDetail();
		void drawArmy();
		void drawDuty(PTMTown * town, std::string title, int dutyEnum);
	private:
		PTMTown * m_currInspectTown = nullptr;
		PTMHero * m_currInspectHero = nullptr;
		PTMNation * m_currInspectNation = nullptr;
		PTMNation * m_currInspectTownListNation = nullptr;
		PTMArmy * m_currInspectArmy = nullptr;
		std::function<void(PTMTown*)> m_townListClickedCB = {};

		PTMNation * m_currInspectHeroListNation = nullptr;
		std::function<void(PTMHero*)> m_heroesListClickedCB = {};
	};

}