#pragma once
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
namespace tzw
{
	struct PTMTile;
	struct PTMTileGraphic
	{
		PTMTileGraphic(PTMTile * m_parent);
		void updateGraphics();
		Sprite * m_sprite;
		PTMTile * m_parent;
	};

	struct PTMTile
	{
		PTMTile();
		unsigned short coord_x;
		unsigned short coord_y;
		PTMTileGraphic * m_graphics;
	};

}