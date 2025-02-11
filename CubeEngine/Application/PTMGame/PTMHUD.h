#pragma once
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
#include "2D/GUIFrame.h"
#include "2D/GUITitledFrame.h"
#include "2D/Button.h"
namespace tzw
{
	class LabelNew;
	struct PTMTile;
	class PTMTown;
	class PTMNation;
	class PTMPlayerController;
	class PTMHUD
	{
	public:
		PTMHUD();
		void init();
		void setController(PTMPlayerController * controller);
		void updateTimeOfDay();
		void updateTimePauseState(bool isPause);
		void updateMonthly();
		void updateResource();
		void updateAll();
	private:
		GUIFrame * m_frame;
		PTMPlayerController * m_controller;
		LabelNew * m_nationName;
		LabelNew * m_gold;
		LabelNew * m_time;
	};

}