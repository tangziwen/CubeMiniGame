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


class PTMHero
{
	PTM_PROPERTY(Name, std::string, 1, "the Grassion of town")
	PTM_PROPERTY(TownLocation, PTMTown * , nullptr, "the location of hero")
	PTM_PROPERTY(CurrRole, PTMHeroRole , PTMHeroRole::Idle, "the location of hero")
	PTM_PROPERTY(Sex, int, 0, "sex") // 0 male, 1 female

	
public:
	PTMHero(std::string Name, int sex);
	const PTMFiveElement & getFiveElement();
	void updateOutputModifier();
	PTMModifier * getOutPutModifier();

protected:
	PTMFiveElement m_FiveElement;
	PTMModifier m_outPutModifier ;//this modifier means this hero affect other things modifers
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