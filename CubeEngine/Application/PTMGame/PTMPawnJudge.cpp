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
	town->m_Garrison -= 75;
	army->m_currSize -= 150.0f;
	if(army->m_currSize <= 0)
	{
		army->m_parent->removeArmy(army);
	}
	if(town->m_Garrison <= 0)//lose
	{
		town->m_Garrison = 0.0f;
		PTMWorld::shared()->getPlayerController()->getControlledNation()->ownTown(town);
	}
}

}
