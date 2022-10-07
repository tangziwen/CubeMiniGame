#include "RLPerk.h"
#include "rapidjson/document.h"
#include "Utility/file/Tfile.h"
#include "Utility/log/Log.h"
#include <algorithm>
#include "Utility/math/TbaseMath.h"
namespace tzw
{



RLPerkMgr::RLPerkMgr()
{
}

void RLPerkMgr::loadPerkList()
{
	std::string filePath = "RL/Perks.json";
	rapidjson::Document doc;
	auto data = Tfile::shared()->getData(filePath, true);
	doc.Parse<rapidjson::kParseTrailingCommasFlag | rapidjson::kParseCommentsFlag>(data.getString().c_str());
	if (doc.HasParseError())
	{
		tlog("[error] get json data err! %s %d offset %d",
			filePath.c_str(),
			doc.GetParseError(),
			doc.GetErrorOffset());
		exit(1);
	}
	for(int i = 0; i < doc.Size(); i++)
	{
		auto& node = doc[i];
		RLPerk * perk = new RLPerk();
		perk->name = node["Name"].GetString();
		perk->title = node["Title"].GetString();
		perk->desc = node["Desc"].GetString();
		m_perkList.push_back(perk);
	}
	
}

void RLPerkMgr::generatePerkCandidates()
{
	int randomNum = 4;
	m_perkCandidates.clear();
	if(m_perkListShuffle.size() >= randomNum )
	{
		for(int i = 0; i < randomNum; i++)
		{
	
			m_perkCandidates.push_back(m_perkListShuffle.back());
			m_perkListShuffle.pop_back();
		}
	}
	else// not enough deck, reshuffle
	{
		startDeck();
		generatePerkCandidates();
	}

	
}

void RLPerkMgr::startDeck()
{
	m_perkListShuffle = m_perkList;
	auto& re = TbaseMath::getRandomEngine();
	std::shuffle(m_perkListShuffle.begin(), m_perkListShuffle.end(), re);
}

void RLPerkMgr::buyOrUpgradePerk(RLPerk* perk)
{
}

std::vector<RLPerk*>& RLPerkMgr::getPerkCandidates()
{
	return m_perkCandidates;
}

}