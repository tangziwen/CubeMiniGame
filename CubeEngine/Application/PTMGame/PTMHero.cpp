#include "PTMHero.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMWorld.h"
#include "PTMPawn.h"
#include "Utility/file/Tfile.h"
#include "PTMGameTimeMgr.h"
namespace tzw
{
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
	if(currDate % 7 == m_tickDayOffset)
	{
		tick_impl(currDate);
	}
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
}

void PTMHero::tick_impl(uint32_t currDate)
{

}

PTMHero* PTMHeroFactory::genRandomHero()
{
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
	hero->updateOutputModifier();
	return hero;
}
void PTMHeroFactory::init()
{
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
