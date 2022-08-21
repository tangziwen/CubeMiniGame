#pragma once
#include <string>
#include <vector>

#include "Engine/EngineDef.h"

namespace tzw
{

	struct RLWeaponData
	{
		std::string m_name;
		float m_damage;
		float m_cd;
		float m_bulletSpeed;
		float m_fireRate;
		int m_fireRound;
		int m_splitNum;
		float m_splitAngle;
		int m_id;
	};
	class RLWeaponCollection : public Singleton<RLWeaponCollection>
	{
	public:
		RLWeaponCollection();
		void loadConfig();
		RLWeaponData * getWeaponData(int id);
		RLWeaponData * getWeaponData(std::string  name);
		int getWeaponIDByName(std::string name);

	private:
		std::vector<RLWeaponData> m_weaponDataCollection;
	};
}
