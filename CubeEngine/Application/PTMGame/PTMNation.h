#pragma once
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
#include "PTMILogicTickable.h"
#include "PTMModifier.h"
#include "PTMBaseDef.h"
#include "PTMObjectReflect.h"
#include "PTMInGameEvent.h"
namespace tzw
{
	class PTMTown;
	class PTMArmy;
	class PTMPawn;
	class PTMTechState;
	class PTMNation : public PTMILogicTickable, public PTMObjectReflect
	{
	PTM_PROPERTY(GlobalModifier, PTMModifierContainer *, nullptr, "the Grassion of town");
	PTM_PROPERTY(TechState, PTMTechState *, nullptr, "the Grassion of town");

	PTM_PROPERTY(MilitaryPoint, float, 0, "Mil mana")
	PTM_PROPERTY(GlobalManPower, float, 0, "Global Man power")
	PTM_PROPERTY(AdminPoint, float, 0, "admin mana")
	PTM_PROPERTY(Gold, float, 0, "admin mana")
	PTM_PROPERTY(NationalYearlyBaseTax, float, 12.f, "admin mana")
	PTM_PROPERTY(NationalYearlyAdimPoint, float, 12.f, "admin mana")
	PTM_PROPERTY(NationalYearlyMilPoint, float, 12.f, "admin mana")
	PTM_PROP_REFLECT_REG_DECLEAR()
		PTM_PROP_REFLECT_REG(MilitaryPoint)
		PTM_PROP_REFLECT_REG(GlobalManPower)
		PTM_PROP_REFLECT_REG(AdminPoint)
		PTM_PROP_REFLECT_REG(Gold)
		PTM_PROP_REFLECT_REG(NationalYearlyBaseTax)
		PTM_PROP_REFLECT_REG(NationalYearlyAdimPoint)
		PTM_PROP_REFLECT_REG(NationalYearlyMilPoint)
	PTM_PROP_REFLECT_REG_END
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
		void addGold(float diff);
		void payGold(float diff);
		bool isCanAfford(float targetGold){return m_Gold >= targetGold;};
		void addArmy(PTMArmy * army);
		void removeArmy(PTMArmy * army);
		std::vector<PTMTown * >& getTownList();
		void addEvent(const PTMInGameEventInstanced  &eventInstanced);
		std::unordered_map<PTMInGameEvent *, PTMInGameEventInstanced> & getEventInstancedList();
	private:
		void garbageCollect();
		uint32_t m_idx = {0};
		std::string m_name;
		vec3 m_nationColor;
		std::vector<PTMTown * > m_townList;
		std::vector<PTMTown * > m_occupyTownList;
		std::vector<PTMArmy * > m_armyList;
		std::vector<PTMPawn * > m_garbages;
		std::unordered_map<PTMInGameEvent *, PTMInGameEventInstanced> m_eventTypePool;
	};

}