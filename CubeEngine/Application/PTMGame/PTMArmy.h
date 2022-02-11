#pragma once
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
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
	class PTMArmy :public EventListener
	{
	public:
		PTMArmy(PTMNation * nation, PTMTile * targetTile);
		void updateGraphics();
		void setTile(PTMTile * targetTile);
		void dailyTick();
	private:
		
		PTMArmyGraphics * m_graphics;
		PTMNation * m_parent;
		PTMTile * m_placedTile;
		friend class PTMArmyGraphics;
	};

}