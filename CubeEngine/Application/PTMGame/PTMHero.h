#pragma once
#include "PTMPawn.h"
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
#include "2D/TileMap2DMgr.h"
#include "PTMBaseDef.h"
#include "PTMModifier.h"

namespace tzw
{
class PTMTown;
enum class PTMHeroRole
{
	Idle,
	Keeper,
	OnDuty,
};

enum class PTMHeroState
{
	Working,
	Playing,
	Illness,
};

enum class PTMHeroMood
{
	Normal,
	Anger,
	Sad,
	Tired,
};

class PTMHero
{
	PTM_PROPERTY(Name, std::string, 1, "the Grassion of town")
	PTM_PROPERTY(TownLocation, PTMTown * , nullptr, "the location of hero")
	PTM_PROPERTY(TownOfKeeper, PTMTown * , nullptr, "the location of hero")
	PTM_PROPERTY(TownOfOnDuty, PTMTown * , nullptr, "the location of hero")
	PTM_PROPERTY(CurrRole, PTMHeroRole , PTMHeroRole::Idle, "the location of hero")
	PTM_PROPERTY(CurrState, PTMHeroState , PTMHeroState::Working, "the location of hero")
	PTM_PROPERTY(CurrMood, PTMHeroMood , PTMHeroMood::Normal, "the location of hero")
	PTM_PROPERTY(Sex, int, 0, "sex") // 0 male, 1 female
public:
	PTMHero(std::string Name, int sex);
	const PTMFiveElement & getFiveElement();
	void updateOutputModifier();
	PTMModifier * getOutPutModifier();
	void onCurrRoleChanged(PTMHeroRole newRole);
	void assignAsKeeper(PTMTown * town);
	void assignOnDuty(PTMTown * town);
	void kickFromDuty();
	void kickFromKeeper();
	void tick(uint32_t currDate);
protected:
	void breakOldDuty();
	void tick_impl(uint32_t currDate);
	int m_tickDayOffset = 0;//reduce the performance drop
	PTMFiveElement m_FiveElement;
	PTMModifier m_outPutModifier ;//this modifier means this hero affect other things modifers
	friend class PTMHeroFactory;
};

class PTMHeroFactory: public Singleton<PTMHeroFactory>
{
public:
	PTMHero * genRandomHero();
	void init();
private:
	std::vector<std::string> m_familyName;
	std::vector<std::string> m_femaleName;
	std::vector<std::string> m_maleName;
};
}