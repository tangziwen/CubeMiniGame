#include "PTMPawn.h"

#include "PTMTile.h"

namespace tzw
{
PTMPawn::PTMPawn()
{
}

PTMTile* PTMPawn::getCurrentTile()
{
	return m_placedTile;
}

void PTMPawn::setTile(PTMTile* tile)
{
	if(m_placedTile)
	{
		m_placedTile->removePawn();
	}
	m_placedTile = tile;
	tile->setPawn(this);
}

PawnTile PTMPawn::getPawnType()
{
	return PawnTile::BASE_PAWN;
}

PTMPawn::~PTMPawn()
{
}
}
