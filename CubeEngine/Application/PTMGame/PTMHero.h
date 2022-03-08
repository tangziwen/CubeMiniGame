#pragma once
#include "PTMPawn.h"
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
#include "2D/TileMap2DMgr.h"
#include "PTMBaseDef.h"
#include "PTMModifier.h"
#include <random>

#include "PTMILogicTickable.h"
#include "PTMTaxPack.h"

namespace tzw
{
class PTMTown;
class PTMNation;
enum class PTMHeroRole
{
	Idle,
	Keeper,
	OnDuty,
	Admin,
	Research,
	Eco,
	Mil,
	OnArmy,
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

enum class DutyObjectiveEnum
{
	Training,
	Developing,
	Working,
};

class PTMHero: public PTMILogicTickable
{
	PTM_PROPERTY(Name, std::string, 1, "the Grassion of town")
	PTM_PROPERTY(TownLocation, PTMTown * , nullptr, "the location of hero")
	PTM_PROPERTY(TownOfKeeper, PTMTown * , nullptr, "the location of hero")
	PTM_PROPERTY(TownOfOnDuty, PTMTown * , nullptr, "the location of hero")
	PTM_PROPERTY(CurrRole, PTMHeroRole , PTMHeroRole::Idle, "the location of hero")
	PTM_PROPERTY(CurrState, PTMHeroState , PTMHeroState::Working, "the location of hero")
	PTM_PROPERTY(CurrMood, PTMHeroMood , PTMHeroMood::Normal, "the location of hero")
	PTM_PROPERTY(Country, PTMNation * , nullptr, "the location of hero")
	PTM_PROPERTY(Level, int , 0, "the location of hero")
	PTM_PROPERTY(DutyObjective, DutyObjectiveEnum , DutyObjectiveEnum::Training, "the location of hero")
	PTM_PROPERTY(Sex, int, 0, "sex") // 0 male, 1 female
public:
	PTMHero(std::string Name, int sex);
	const PTMFiveElement & getFiveElement();
	void updateOutputModifier();
	PTMModifier * getOutPutModifier();
	void onCurrRoleChanged(PTMHeroRole newRole);


	void assignAsKeeper(PTMTown * town);
	void assignOnDuty(PTMTown * town);
	void assignResearch();
	void assignAdm();
	void assignEco();
	void assignMil();
	void kickFromDuty();
	void kickFromKeeper();
	void tick(uint32_t currDate);

	virtual void onMonthlyTick() override;
	virtual void onDailyTick() override;
	virtual void onWeeklyTick() override;
	void breakOldDuty();
	const PTMTaxPack collectUpKeep();
protected:
	
	void tick_impl(uint32_t currDate);
	int m_tickDayOffset = 0;//reduce the performance drop
	PTMFiveElement m_FiveElement;
	PTMModifier m_outPutModifier ;//this modifier means this hero affect other things modifers
	friend class PTMHeroFactory;
	PTMTaxPack m_upKeep;
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
	std::default_random_engine m_generator;
	std::normal_distribution<float> * m_heroAttritdist;
};
}
