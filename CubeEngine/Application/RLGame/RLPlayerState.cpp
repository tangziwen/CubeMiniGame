#include "RLPlayerState.h"
#include "RLCollectible.h"
#include "RLWorld.h"
namespace tzw
{
RLPlayerState::RLPlayerState()
{
	reset();
}
void RLPlayerState::reset()
{
	m_score = 0;
	m_currLevel = 1;
	m_currExp = 0;
	m_maxExp = calculateNextLevelExp(m_currLevel + 1);
}
unsigned int RLPlayerState::getScore()
{
	return m_score;
}
void RLPlayerState::setScore(unsigned int newScore)
{
	m_score = newScore;
}
void RLPlayerState::addScore(unsigned int addedScore)
{
	m_score += addedScore;
}
unsigned int RLPlayerState::calculateNextLevelExp(unsigned int nextLevel)
{
	return (m_currLevel + 1) * 25 + 500;
}
void RLPlayerState::gainExp(unsigned int exp)
{
	m_currExp += exp;
	if(m_currExp >= m_maxExp)
	{
		m_currLevel++;//level Up
		RLWorld::shared()->generateLevelUpPerk();
		m_currExp = m_currExp - m_maxExp;
		m_maxExp = calculateNextLevelExp(m_currLevel);
	}
}
unsigned int RLPlayerState::getCurrLevel()
{
	return m_currLevel;
}
unsigned int RLPlayerState::getCurrExp()
{
	return m_currExp;
}
unsigned int RLPlayerState::getMaxExp()
{
	return m_maxExp;
}
}
