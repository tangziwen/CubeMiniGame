#pragma once
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
#include "PTMILogicTickable.h"
namespace tzw
{
	class PTMTown;
	class PTMArmy;
	class PTMPawn;
	class PTMNation : public PTMILogicTickable
	{
	public:
		PTMNation();
		void millitaryOccupyTown(PTMTown * town); //
		void ownTown(PTMTown * town);
		void setName(std::string name) { m_name = name; }
		std::string getName() { return m_name; }
		void setNationColor(vec3 nationColor) { m_nationColor = nationColor; }
		vec3 getNationColor() { return m_nationColor; }
		uint32_t getIdx() {return m_idx;};
		void setIdx(uint32_t newIdx) {m_idx = newIdx;};
		virtual void onMonthlyTick() override;
		virtual void onDailyTick() override;
		void updateTownsMonthly();
		void updateArmiesMonthly();
		float getGold() {return m_gold;};
		void addGold(float diff);
		void payGold(float diff);
		bool isCanAfford(float targetGold){return m_gold >= targetGold;};
		int getManpower() {return m_manpower;};
		void addArmy(PTMArmy * army);
		void removeArmy(PTMArmy * army);
	private:
		void garbageCollect();
		uint32_t m_idx = {0};
		std::string m_name;
		vec3 m_nationColor;
		std::vector<PTMTown * > m_townList;
		std::vector<PTMTown * > m_occupyTownList;
		std::vector<PTMArmy * > m_armyList;
		std::vector<PTMPawn * > m_garbages;
		float m_gold = {0.0f};
		int m_manpower = {0};
	};

}