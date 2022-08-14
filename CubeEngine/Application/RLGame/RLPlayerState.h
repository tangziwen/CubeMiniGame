#pragma once
#include "Engine/EngineDef.h"
#include <unordered_set>
namespace tzw
{
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
		void addGold(unsigned int newGoldValue);
		void unlockHero(std::string heroName);
		bool isHeroUnLock(std::string heroName);
	private:
		unsigned int m_score;
		unsigned int m_currLevel;
		unsigned int m_currExp;
		unsigned int m_maxExp;
		unsigned int m_gold = 0;
		std::unordered_set<std::string> m_heroPurchasedMap;
	};
}
