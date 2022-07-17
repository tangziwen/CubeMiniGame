#include "RLPlayerState.h"

namespace tzw
{
RLPlayerState::RLPlayerState()
{
	reset();
}
void RLPlayerState::reset()
{
	m_score = 0;
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
}
