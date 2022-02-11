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
		PTMArmy * m_parent;
		Sprite * m_sprite;
		LabelNew * m_label;
		GUIFrame * m_selectedBorders[4];
		Button * m_button;
		
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
		uint32_t m_currSize;
		uint32_t m_sizeLimit;
		PTMArmyGraphics * m_graphics;
		PTMNation * m_parent;
		PTMTile * m_placedTile;
		friend class PTMArmyGraphics;
	};

}