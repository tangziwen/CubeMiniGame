#pragma once
#include "PTMPawn.h"
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
namespace tzw
{
	struct PTMTile;
	class PTMTown;
	struct PTMTileGraphic
	{
		PTMTileGraphic(PTMTile * m_parent);
		void updateGraphics();
		Sprite * m_sprite = nullptr;
		PTMTile * m_parent = nullptr;
	};

	enum class PTMTileType
	{
		TILE_NORMAL,
		TILE_OCEAN,
		TILE_WASTE_LAND,
	};
	struct PTMTile
	{
		PTMTile();
		unsigned short coord_x{0};
		unsigned short coord_y{0};
		PTMTileGraphic * m_graphics = nullptr;
		vec2 getCanvasPos();

		void setPawn(PTMPawn * pawn);
		void removePawn();
		PTMPawn * getPawn() {return m_pawn;}
		PTMPawn * m_pawn = nullptr;
		PTMTown * m_owner = nullptr;
		PTMTileType m_tileType {PTMTileType::TILE_NORMAL};
	};

}