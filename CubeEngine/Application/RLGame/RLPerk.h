#include "Engine/EngineDef.h"
#include <vector>
namespace tzw
{
	struct RLPerk
	{
		std::string name;
		std::string title;
		std::string desc;
	};

	struct RLPerkInstance
	{
		RLPerkInstance(RLPerk * perk):m_perk(perk), level(0){}
		RLPerk * m_perk;
		int level;
	};


	struct RLPerkInstContainer
	{
		void addPerk(RLPerk * perk)
		{
			auto iter = m_instMap.find(perk);
			if(iter == m_instMap.end())
			{
				RLPerkInstance * inst = new RLPerkInstance(perk);
				m_instMap[perk] = inst;
			}else
			{
				iter->second->level += 1;
			}
		}
		RLPerkInstance * hasPerk(RLPerk * perk)
		{
			auto iter = m_instMap.find(perk);
			if(iter == m_instMap.end())
			{
				return nullptr;
			}
			else
			{
				return iter->second;
			}
		}
		std::unordered_map<RLPerk *, RLPerkInstance *> m_instMap;
	};

	class RLPerkMgr : public Singleton<RLPerkMgr>
	{
	public:
		RLPerkMgr();
		void loadPerkList();
		void generatePerkCandidates();
		void startDeck();
		void buyOrUpgradePerk(RLPerk * perk);
		std::vector<RLPerk *> & getPerkCandidates();
		unsigned int m_currSelectIdx = 0;//quick variable for gui
	private:
		std::vector<RLPerk *> m_perkList;
		std::vector<RLPerk *> m_perkListShuffle;
		std::vector<RLPerk *> m_perkCandidates;
	
	

	};
}