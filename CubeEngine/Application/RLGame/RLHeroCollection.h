#pragma once
#include <string>
#include <vector>

#include "Engine/EngineDef.h"

namespace tzw
{

	struct RLHeroData
	{
		std::string m_name;
		float m_maxHealth;
		float m_meleeDamage;
		float m_speed;
		float m_difficulty;
		int m_tier;
		std::string m_defaultWeapon;
		std::string m_sprite;
		std::string m_aiType;

	};
	class RLHeroCollection : public Singleton<RLHeroCollection>
	{
	public:
		RLHeroCollection();
		void loadConfig();
		RLHeroData * getHeroData(int id);
		RLHeroData * getHeroData(std::string  name);
		int getHeroIDByName(std::string name);
		const std::vector<RLHeroData> & getPlayableHeroDatas();
		void getHeroRangeFromTier(int Tier, std::vector<RLHeroData*>& data);
	private:
		void loadConfigImpl(std::string filePath, bool isPlayable);
		std::vector<RLHeroData> m_heroDataCollection;
		std::vector<RLHeroData> m_playableHeroDataCollection;
		std::vector<RLHeroData> m_MonsterHeroDataCollection;
	};
}
