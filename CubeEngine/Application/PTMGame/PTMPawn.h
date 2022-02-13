#pragma once

namespace tzw
{
	struct PTMTile;
	class PTMPawnJudge;
	enum class PawnTile
	{
		BASE_PAWN,
		ARMY_PAWN,
		TOWN_PAWN,
		BUILDING_PAWN,
	};
	class PTMPawn
	{
	public:
		PTMPawn();
		PTMTile * getCurrentTile();
		virtual void setTile(PTMTile * tile);
		virtual PawnTile getPawnType();
		virtual ~PTMPawn();
	protected:
		PTMTile * m_placedTile = nullptr;
	};

}