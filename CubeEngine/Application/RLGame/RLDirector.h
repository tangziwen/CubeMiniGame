#pragma once
#include "Engine/EngineDef.h"
#include <vector>
#include <unordered_map>
namespace tzw
{
	class RLSubWave
	{
	public:
		void startWave();
		TZW_PROPERTY(float, WaitingTime, 25.0f);
		std::unordered_map<int, int> & getGeneratedMonsterList(){return m_generateMonster;};
	private:
		std::unordered_map<int, int> m_generateMonster;
	
	};
	class RLWave
	{
	public:
		
		void tick(float dt);
		void generateSubWaves();
		bool isFinished();
	private:
		std::vector<RLSubWave*> m_SubWaveList;
		TZW_PROPERTY(int, SubWaveIndex, 0);
		float m_time = 0.f;
	};
	class RLDirector : public Singleton<RLDirector>
	{
	public:
		RLDirector();
		void tick(float dt);
		void startWave();
		void generateWave();
		int getCurrentWave();
		int getCurrentSubWave();
		bool isFinished();
	private:
		float m_time = 0.f;
		std::vector<RLWave *> m_waveList;
		int m_waveIndex = 0;
	
	};
}