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
	private:
		unsigned int m_score;

	};
}
