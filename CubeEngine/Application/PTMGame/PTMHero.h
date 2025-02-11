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
class PTMDepartment;
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
	MAX_NUM_DUTITES
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
	PTM_PROPERTY(CurrExp, unsigned int , 0, "the location of hero")
	PTM_PROPERTY(DutyObjective, DutyObjectiveEnum , DutyObjectiveEnum::Training, "the location of hero")
	PTM_PROPERTY(Sex, int, 0, "sex") // 0 male, 1 female
	PTM_PROPERTY(DutyProgress, int, 0, "DutyProgress")
	PTM_PROPERTY(CurrDepartment, PTMDepartment * , nullptr, "CurrDepartment")
public:
	PTMHero(std::string Name, int sex);
	const PTMFiveElement & getFiveElement();
	void updateOutputModifier();
	PTMModifier * getOutPutModifier();
	void onCurrRoleChanged(PTMHeroRole newRole);


	void assignAsKeeper(PTMTown * town);
	void assignOnDuty(PTMTown * town, DutyObjectiveEnum objective);
	void assignResearch();
	void assignAdm();
	void assignEco();
	void assignMil();
	void kickFromDuty();
	void kickFromKeeper();
	void tick(uint32_t currDate);
	void payDay(float val);
	float getEstimateLevelUpMonth();

	virtual void onMonthlyTick() override;
	virtual void onDailyTick() override;
	virtual void onWeeklyTick() override;
	void breakOldDuty();
	const PTMTaxPack collectUpKeep();
	void updateDuty();
	int getDutyProgressMax();
	float getUpKeep();
	unsigned int getMaxEXP();
	void gainExp(unsigned int exp);
protected:
	float getDutyUpKeep();
	void tick_impl(uint32_t currDate);
	int m_tickDayOffset = 0;//reduce the performance drop
	PTMFiveElement m_FiveElement;
	PTMModifier m_outPutModifier ;//this modifier means this hero affect other things modifers
	friend class PTMHeroFactory;
	PTMTaxPack m_upKeep;
	float m_estimatedMonth = 10000.0f;
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
