#include "PTMHero.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMWorld.h"
#include "PTMPawn.h"
#include "Utility/file/Tfile.h"

namespace tzw
{
PTMHero::PTMHero(std::string Name, int sex)
{
	m_Name = Name;
	m_Sex = sex;
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
}
