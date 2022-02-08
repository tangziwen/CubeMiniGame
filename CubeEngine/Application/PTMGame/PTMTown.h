#pragma once
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
namespace tzw
{
	struct PTMTile;
	class PTMTown;
	class PTMTownGraphics
	{
	public:
		PTMTownGraphics(PTMTown * parent);
		void updateGraphics();
	private:
		PTMTown * m_parent;
		Sprite * m_townSprite;
		
	};
	class PTMTown :public EventListener
	{
	public:
		PTMTown(PTMTile * placedTile);
		void updateGraphics();
	private:
		PTMTile * m_placedTile;
		std::string m_name;
		PTMTownGraphics * m_graphics;
		friend class PTMTownGraphics;
	};

}