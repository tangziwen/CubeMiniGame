#pragma once
#include "Engine/EngineDef.h"
#include <unordered_set>

#define RL_STAT_STR 0
#define RL_STAT_DEX 1
#define RL_STAT_INT 2
namespace tzw
{
	struct RLPerkInstContainer;
	enum class HeroPurchaseAction
	{
		Upgrade,
		Unlock
	};
	class RLPlayerState : public Singleton<RLPlayerState>
	{
	public:
		RLPlayerState();
		void reset();
		unsigned int getScore();
		void setScore(unsigned int newScore);
		void addScore(unsigned int addedScore);
		unsigned int calculateNextLevelExp(unsigned int nextLevel);
		void gainExp(unsigned int exp);
		unsigned int getCurrLevel();
		unsigned int getCurrExp();
		unsigned int getMaxExp();
		unsigned int getGold() {return m_gold;};

		int getStat(int index) 
		{
			return m_stats[index];
		}
		void setStat(int index, int val)
		{
			m_stats[index] = val;
		}
		void addStat(int index, int val)
		{
			m_stats[index] += val;
		}
		void addGold(unsigned int newGoldValue);
		void unlockHero(std::string heroName);
		bool isHeroUnLock(std::string heroName);
		int getHeroLevel(std::string heroName);
		void heroUpdate(std::string heroName);
		bool isCanAffordHeroPurchase(std::string heroName, HeroPurchaseAction);
		void loadPersistent();
		void writePersistent();
		RLPerkInstContainer * getPerkContainer();
	private:
		unsigned int m_score;
		unsigned int m_currLevel;
		unsigned int m_currExp;
		unsigned int m_maxExp;
		unsigned int m_gold = 0;
		int m_stats[3] = {0, 0, 0};
		std::unordered_map<std::string, int> m_heroPurchasedMap;
		RLPerkInstContainer *  m_PerkContainer;
	};
}
