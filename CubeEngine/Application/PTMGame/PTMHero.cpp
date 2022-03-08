#include "PTMHero.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMWorld.h"
#include "PTMPawn.h"
#include "Utility/file/Tfile.h"
#include "PTMGameTimeMgr.h"
#include <array>
#include <algorithm>
#include "PTMConfig.h"
namespace tzw
{
constexpr float MEAN_POINT_BASE = 30.f;
constexpr float POINT_MAX = 45.f;
constexpr float POINT_MIN = 15.0f;
PTMHero::PTMHero(std::string Name, int sex)
{
	m_Name = Name;
	m_Sex = sex;
	m_tickDayOffset = rand() % 7;
}

const PTMFiveElement& PTMHero::getFiveElement()
{
	return m_FiveElement;

}

void PTMHero::updateOutputModifier()
{
	m_outPutModifier.reset();
	float rate = 1.f;
	if(m_CurrRole == PTMHeroRole::Keeper) rate = 1.5f;

}
PTMModifier* PTMHero::getOutPutModifier()
{
	return &m_outPutModifier;
}
void PTMHero::onCurrRoleChanged(PTMHeroRole newRole)
{
	if(m_CurrRole == PTMHeroRole::Keeper)
	{
		m_TownOfKeeper->setKeeper(nullptr);
	}
	if(m_CurrRole == PTMHeroRole::OnDuty)
	{
		m_TownOfKeeper->setKeeper(nullptr);
	}
	printf("hehehe");
}

void PTMHero::assignAsKeeper(PTMTown* town)
{
	breakOldDuty();
	town->setKeeper(this);
	this->m_TownOfKeeper = town;
	this->setCurrRole(PTMHeroRole::Keeper);
}

void PTMHero::assignOnDuty(PTMTown* town)
{
	breakOldDuty();
	town->assignOnDuty(this);
	m_TownOfOnDuty = town;
	setCurrRole(PTMHeroRole::OnDuty);
}

void PTMHero::assignResearch()
{
	breakOldDuty();
	setCurrRole(PTMHeroRole::Research);
}

void PTMHero::assignAdm()
{
	breakOldDuty();
	setCurrRole(PTMHeroRole::Admin);
}

void PTMHero::assignEco()
{
	breakOldDuty();
	setCurrRole(PTMHeroRole::Eco);
}

void PTMHero::assignMil()
{
	breakOldDuty();
	setCurrRole(PTMHeroRole::Mil);
}

void PTMHero::kickFromDuty()
{
	m_TownOfOnDuty->kickOnDuty(this);
	m_TownOfOnDuty = nullptr;
	setCurrRole(PTMHeroRole::Idle);
	return ;
}

void PTMHero::kickFromKeeper()
{
	m_TownOfKeeper->setKeeper(nullptr);
	m_TownOfKeeper = nullptr;
	setCurrRole(PTMHeroRole::Idle);
}

void PTMHero::tick(uint32_t currDate)
{

}

void PTMHero::onMonthlyTick()
{
	switch(m_CurrRole)
	{
	case PTMHeroRole::Idle:
		m_upKeep.m_gold += G_PTMConfigMgr["GlobalConst"]["HeroUpKeep_Idle"];
		break;
	case PTMHeroRole::Keeper:
		m_upKeep.m_gold += G_PTMConfigMgr["GlobalConst"]["HeroUpKeep_Keeper"];
		break;
	case PTMHeroRole::OnDuty:
		m_upKeep.m_gold += G_PTMConfigMgr["GlobalConst"]["HeroUpKeep_Duty"];
		break;
	case PTMHeroRole::Admin:
		m_upKeep.m_gold += G_PTMConfigMgr["GlobalConst"]["HeroUpKeep_Duty"];
		break;
	case PTMHeroRole::Research:
		break;
	case PTMHeroRole::Eco:
		break;
	case PTMHeroRole::Mil:
		break;
	}
	
}

void PTMHero::onDailyTick()
{
	uint32_t currDate = PTMGameTimeMgr::shared()->getCurrDate();
	if(currDate % 7 == m_tickDayOffset)
	{
		tick_impl(currDate);
	}
}

void PTMHero::onWeeklyTick()
{
}

void PTMHero::breakOldDuty()
{
	if(m_TownOfKeeper)
	{
		kickFromKeeper();
	}
	if(m_TownOfOnDuty)
	{
		kickFromDuty();
	}
	if(m_CurrRole == PTMHeroRole::Admin)
	{
		m_Country->kickAdmHero(this);
	}
	if(m_CurrRole == PTMHeroRole::Eco)
	{
		m_Country->kickEcoHero(this);
	}
	if(m_CurrRole == PTMHeroRole::Mil)
	{
		m_Country->kickMilHero(this);
	}
	if(m_CurrRole == PTMHeroRole::Research)
	{
		m_Country->kickResearchHero(this);
	}
}

const PTMTaxPack PTMHero::collectUpKeep()
{
	PTMTaxPack tmp = m_upKeep;
	m_upKeep.reset();
	return tmp;
}

void PTMHero::tick_impl(uint32_t currDate)
{

}

PTMHero* PTMHeroFactory::genRandomHero()
{
	auto& engine = TbaseMath::getRandomEngine();

	int sex = rand() % 2;
	std::string firstName = "";
	std::string familyName = m_familyName[rand()%m_familyName.size()];
	if (sex == 0)
	{
		firstName = m_maleName[rand()%m_maleName.size()];
	}
	if (sex == 1)
	{
		firstName = m_femaleName[rand()%m_femaleName.size()];
	}
	char tmpBuff[64];
	sprintf(tmpBuff, "%s.%s", familyName.c_str(), firstName.c_str());
	auto hero = new PTMHero(tmpBuff, sex);
	float totalPoint = 0.f;
	do{
		totalPoint= (*m_heroAttritdist)(m_generator);
	}while( totalPoint<POINT_MIN || totalPoint>POINT_MAX );

	std::normal_distribution<float> levelDist(2.f, 1.5f);
	hero->m_Level = std::clamp<int>(levelDist(engine), 0, 3);

	totalPoint += hero->m_Level * (10 + engine() % 3);
	std::array<int, 5> idx = {0, 1, 2, 3, 4};
	std::shuffle(idx.begin(), idx.end(), engine);
	
	int mainElement = idx[0];
	std::normal_distribution<float> dist(totalPoint / 5.0, 1.5f);
	for(int i = 0; i < idx.size(); i++)
	{

		if(i == idx.size() - 1)//last one
		{
			hero->m_FiveElement[idx[i]] = totalPoint;
		}
		else
		{
			int point = std::clamp<int>(dist(engine), 1, totalPoint * 0.75f);
			hero->m_FiveElement[idx[i]] = point;
			totalPoint -= point;
		}

	}
	hero->updateOutputModifier();
	return hero;
}
void PTMHeroFactory::init()
{
	m_heroAttritdist = new std::normal_distribution<float>(MEAN_POINT_BASE, 1.5f);

	std::string filePath = "PTM/data/Hero/NameList.json";
	rapidjson::Document doc;
	auto data = Tfile::shared()->getData(filePath, true);
	doc.Parse<rapidjson::kParseCommentsFlag | rapidjson::kParseTrailingCommasFlag>(data.getString().c_str());
	if (doc.HasParseError())
	{
		tlog("[error] get json data err! %s %d offset %d",
			filePath.c_str(),
			doc.GetParseError(),
			doc.GetErrorOffset());
		exit(1);
	}

	auto& familyNames = doc["FamilyNames"];
	for(auto i = 0; i < familyNames.Size(); i++)
	{
		auto& nameDoc = familyNames[i];

		m_familyName.push_back(nameDoc.GetString());
	}

	auto& femaleNames = doc["FemaleNames"];
	for(auto i = 0; i < femaleNames.Size(); i++)
	{
		auto& nameDoc = femaleNames[i];

		m_femaleName.push_back(nameDoc.GetString());
	}

	auto& maleNames = doc["MaleNames"];
	for(auto i = 0; i < maleNames.Size(); i++)
	{
		auto& nameDoc = maleNames[i];
		m_maleName.push_back(nameDoc.GetString());
	}
}
}
