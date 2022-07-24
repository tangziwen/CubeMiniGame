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
		std::string m_sprite;

	};
	class RLHeroCollection : public Singleton<RLHeroCollection>
	{
	public:
		RLHeroCollection();
		void loadConfig();
		RLHeroData * getHeroData(int id);
		RLHeroData * getHeroData(std::string  name);
		int getHeroIDByName(std::string name);

	private:
		std::vector<RLHeroData> m_heroDataCollection;
	};
}
