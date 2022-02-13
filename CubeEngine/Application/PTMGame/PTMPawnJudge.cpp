#include "PTMPawnJudge.h"
#include "PTMPawn.h"
#include "PTMTile.h"
#include "PTMArmy.h"
#include "PTMTown.h"
#include "PTMWorld.h"
namespace tzw
{
PTMPawnJudge::PTMPawnJudge()
{
}

void PTMPawnJudge::offensive(PTMPawn* attacker, PTMPawn* defender)
{
	if(attacker->getPawnType() == PawnTile::ARMY_PAWN && defender->getPawnType() == PawnTile::TOWN_PAWN)
	{
		offensive_siege_impl(attacker, defender);
	}
}

void PTMPawnJudge::offensive_siege_impl(PTMPawn* attacker, PTMPawn* defender)
{
	auto army = static_cast<PTMArmy*>(attacker);
	auto town = static_cast<PTMTown *>(defender);

	float dmg = army->m_currSize * 0.3f;
	town->m_garrison -= 200;
	army->m_currSize -= 150.0f;
	if(town->m_garrison <= 0)//lose
	{
		PTMWorld::shared()->getPlayerController()->getControlledNation()->ownTown(town);
		town->m_garrison = 0.0f;
	}
}

}
