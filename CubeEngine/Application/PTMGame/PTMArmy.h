#pragma once
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
#include "PTMILogicTickable.h"
namespace tzw
{
	class LabelNew;
	struct PTMTile;
	class PTMTown;
	class PTMNation;
	class PTMArmy;
	class GUIFrame;
	class Button;
	class PTMArmyGraphics
	{
	public:
		PTMArmyGraphics(PTMArmy * parent);
		void updateGraphics();
	private:
		PTMArmy * m_parent = nullptr;
		Sprite * m_sprite = nullptr;
		LabelNew * m_label = nullptr;
		GUIFrame * m_selectedBorders[4] {nullptr};
		Button * m_button {nullptr};
		
	};
	class PTMArmy : public PTMILogicTickable
	{
	public:
		PTMArmy(PTMNation * nation, PTMTile * targetTile);
		void updateGraphics();
		void setTile(PTMTile * targetTile);
		virtual void onMonthlyTick() override;
		virtual void onDailyTick() override;
	private:
		uint32_t m_currSize = 0;
		uint32_t m_sizeLimit = 0;
		PTMArmyGraphics * m_graphics = nullptr;
		PTMNation * m_parent = nullptr;
		PTMTile * m_placedTile = nullptr;
		friend class PTMArmyGraphics;
	};

}