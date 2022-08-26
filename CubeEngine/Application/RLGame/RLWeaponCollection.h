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
		//spray
		float m_sprayAngle = 60.0f;
		float m_sprayTime = 2.0f;
		//spray discrete
		int m_splitAngleCount = 3;
		int m_shiftBulletCount = 3;
		//circle
		float m_circleTime = 2.0f;

		std::string AimPolicyType;
		bool m_isCanChangeDirWhilstShoot;
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
