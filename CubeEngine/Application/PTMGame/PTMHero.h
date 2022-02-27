#pragma once
#include "PTMPawn.h"
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
#include "2D/TileMap2DMgr.h"
#include "PTMBaseDef.h"


namespace tzw
{
class PTMTown;
class PTMHero
{
	PTM_PROPERTY(FirstName, std::string, 1, "the Grassion of town")
	PTM_PROPERTY(FamilyName, std::string, 1, "the Grassion of town")
	PTM_PROPERTY(TownLocation, PTMTown * , nullptr, "the location of hero")
	PTM_PROPERTY(Sex, int, 0, "sex") // 0 male, 1 female
public:
	PTMHero(std::string FamilyName, std::string FirstName, int sex);
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