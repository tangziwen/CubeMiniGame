#pragma once
#include "Engine/EngineDef.h"
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
	private:
		unsigned int m_score;
		unsigned int m_currLevel;
		unsigned int m_currExp;
		unsigned int m_maxExp;

	};
}
