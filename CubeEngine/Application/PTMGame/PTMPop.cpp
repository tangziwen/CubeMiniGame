#include "PTMPop.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMWorld.h"
#include "PTMPawn.h"
#include "Utility/file/Tfile.h"

namespace tzw
{
void PTMPopFactory::init()
{
	loadJobs();
	loadRaces();
}
void PTMPopFactory::loadRaces()
{
	std::string filePath = "PTM/data/Pop/PopRace.json";
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

	auto& races = doc["PopRaces"];
	for(auto i = 0; i < races.Size(); i++)
	{
		auto& raceDoc = races[i];
		PTMPopRaceType race;
		race.setName(raceDoc["Name"].GetString());
		race.setFoodConsume(raceDoc["FoodConsume"].GetFloat());
		race.setEveryDayNeedsConsume(raceDoc["EveryDayNeedsConsume"].GetFloat());
		race.setLuxuryGoodsConsume(raceDoc["LuxuryGoodsConsume"].GetFloat());
		m_races.push_back(race);
	}
}

void PTMPopFactory::loadJobs()
{
	std::string filePath = "PTM/data/Pop/PopJob.json";
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

	auto& jobs = doc["PopJobs"];
	for(auto i = 0; i < jobs.Size(); i++)
	{
		auto& jobDoc = jobs[i];
		PTMPopJobType popjob;
		popjob.setName(jobDoc["Name"].GetString());

		popjob.setFoodProduct(jobDoc["FoodProduct"].GetFloat());
		popjob.setEveryDayNeedsProduct(jobDoc["EveryDayNeedsProduct"].GetFloat());
		popjob.setLuxuryGoodsProduct(jobDoc["LuxuryGoodsProduct"].GetFloat());

		if(jobDoc.HasMember("GoldProduct"))
		{
			popjob.setGoldProduct(jobDoc["GoldProduct"].GetFloat());
		}
		if(jobDoc.HasMember("AdmProduct"))
		{
			popjob.setAdmProduct(jobDoc["AdmProduct"].GetFloat());
		}
		popjob.m_JobType = jobDoc["JobType"].GetInt();
		m_jobs.push_back(popjob);
	}
}

void PTMPopFactory::createAPop(PTMPop* pop, int jobType)
{

	pop->m_job = &m_jobs[jobType];
	pop->m_race = &m_races[1];
	pop->m_happiness = 0.6f;
}

void PTMPop::selfUpgradeMaybe()
{
	int upgradeRate = 15;
	auto& re = TbaseMath::getRandomEngine();
	if((re() % 100) > (100 - upgradeRate))
	{
		m_happinessLevel = std::clamp(m_happinessLevel + 1, POP_MIN_HAPPY_LEVEL, POP_MAX_HAPPY_LEVEL);
		m_happiness = 0.5f;
	}
	else
	{
		m_happiness = 1.f;
	}
}

}
