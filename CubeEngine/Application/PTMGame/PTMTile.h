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
		Sprite * m_sprite = nullptr;
		PTMTile * m_parent = nullptr;
	};

	struct PTMTile
	{
		PTMTile();
		unsigned short coord_x{0};
		unsigned short coord_y{0};
		PTMTileGraphic * m_graphics = nullptr;
		vec2 getCanvasPos();
	};

}