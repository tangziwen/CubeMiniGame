#ifndef S_GAME_SYSTEM_H
#define S_GAME_SYSTEM_H
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include "Engine/EngineDef.h"
#include <string>
#include "s_City.h"
#include "s_Faction.h"
#include <vector>
namespace tzw
{
	class s_GameSystem
	{
	public:
		TZW_SINGLETON_DECL(s_GameSystem);
		void start(std::string scenario);
		void loadCities();
		void loadScenario(std::string scenario);
	private:
		std::vector<s_City *> m_cityList;
		std::vector<s_Faction *> m_factionList;
		s_GameSystem();

	};

}
#endif // !S_GAME_SYSTEM_H
