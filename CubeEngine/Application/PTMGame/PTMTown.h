#pragma once
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
namespace tzw
{
	struct PTMTile;
	class PTMTown;
	class PTMNation;
	class PTMTownGraphics
	{
	public:
		PTMTownGraphics(PTMTown * parent);
		void updateGraphics();
	private:
		PTMTown * m_parent;
		Sprite * m_townSprite;
		Sprite * m_flagSprite;
		
	};
	class PTMTown :public EventListener
	{
	public:
		PTMTown(PTMTile * placedTile);
		void updateGraphics();
		void setName(std::string name) { m_name = name; }
		std::string getName() { return m_name; }
	private:
		PTMTile * m_placedTile;
		std::string m_name;
		PTMNation * m_occupant;
		PTMNation * m_owner;
		PTMTownGraphics * m_graphics;
		friend class PTMTownGraphics;
		friend class PTMNation;
	};

}