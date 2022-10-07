#pragma once
#include "Engine/EngineDef.h"
#include "RLWeaponCollection.h"
namespace tzw
{
	class RLShopMgr : public Singleton<RLShopMgr>
	{
		public:
			RLShopMgr();
			void init();
			void open(int subwaveIdx);
			bool isOpen();
			void close();
			void draw();
		private:
			bool m_isOpen = false;
			int m_currWave = 0;
			std::vector<std::vector<RLWeaponData*>> m_weaponTierList;
			std::vector<int> m_weaponSold;
			void * currSelect = nullptr;
			int selectType;
	};


}