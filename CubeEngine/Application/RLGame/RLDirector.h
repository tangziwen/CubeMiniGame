#pragma once
#include "Engine/EngineDef.h"
#include <vector>
#include <unordered_map>
namespace tzw
{
	struct RLEnemyGenerateionInfo
	{
		std::string m_monsterName;
		int m_maxCount;
		int weight;
	};

	struct RLSubWaveGenerator
	{

		std::vector<RLEnemyGenerateionInfo> m_info;
		void generate(int targetDifficulty, int targetMaxCount, std::unordered_map<int, int> & generateMonster, int & totalCount);
		void addMonstersByTiers(int count, int tier, bool ignoreMob);
		void removeMonstersByTiers(int tier);
		void removeAllMonsters();
	};

	class RLSubWave
	{
	
	public:
		
		RLSubWave(int wave, int subwaveID);
		void startWave();
		std::unordered_map<int, int> & getGeneratedMonsterList(){return m_generateMonster;};
		TZW_PROPERTY(float, WaitingTime, 25.0f);
		TZW_PROPERTY(bool, IsLaunch, false);
	private:
		std::unordered_map<int, int> m_generateMonster;
		int m_totalCount = 0;
		friend class RLWave;
	
	};

	struct RLMonsterGroupInfo
	{
		std::string typeName;
		int tier;
		int num;
	};

	struct RLWaveGroupInfo
	{
		std::vector<RLMonsterGroupInfo> m_groupInfo;
		float m_delayTime = 0.f;
	};

	class RLWave
	{
	public:
		RLWave(int wave):m_waveId(wave){};
		void loadStage(std::string filePath);
		void tick(float dt);
		void generateSubWaves();
		bool isFinished();
		void openPerkSelction();
		void launchNextSubWave();
		void regKill(int heroID);
		bool isFinishedCurrSubWave();
		void startNextSubWave();
	private:
		std::unordered_map<int, int> m_killRegs;
		std::vector<RLSubWave*> m_SubWaveList;
		TZW_PROPERTY(int, SubWaveIndex, 0);
		float m_time = 0.f;
		int m_waveId = 0;
		std::vector<RLWaveGroupInfo> m_importantWaveGroup;
		std::vector<RLWaveGroupInfo> m_specialRandomWaves;
		
	};
	class RLDirector : public Singleton<RLDirector>
	{
	public:
		RLDirector();
		void tick(float dt);
		void startWave();
		void startNextSubWave();
		void generateWave();
		int getCurrentWave();
		int getCurrentSubWave();
		bool isFinished();
		float getCombatStrength()  {return m_combatStrength;};
		void addCombatStrength(float val) {m_combatStrength += val;};
		RLWave* getCurrentWavePtr() {return m_waveList[m_waveIndex]; };
	private:
		float m_time = 0.f;
		std::vector<RLWave *> m_waveList;
		int m_waveIndex = 0;
		float m_combatStrength = 0.f;
	
	};
}