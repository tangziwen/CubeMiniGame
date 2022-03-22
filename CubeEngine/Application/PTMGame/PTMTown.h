#pragma once
#include "Engine/EngineDef.h"
#include "PTMBaseDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
#include "PTMILogicTickable.h"
#include "PTMPawn.h"
#include "PTMPop.h"
#include "PTMModifier.h"
#include "PTMTaxPack.h"
#include "PTMHero.h"
namespace tzw
{
	class LabelNew;
	class Button;
	struct PTMTile;
	class PTMTown;
	class PTMNation;
	class PTMHero;

	struct PTMPopOutputView
	{
		float m_FoodInput = 0.f;
		float m_FoodOutput = 0.f;

		float m_EveryDayNeedsInput = 0.f;
		float m_EveryDayNeedsOutput = 0.f;

		float m_LuxuryGoodsInput = 0.f;
		float m_LuxuryGoodsOutput = 0.f;

		void reset()
		{
			m_FoodInput = 0.f;
			m_FoodOutput = 0.f;

			m_EveryDayNeedsInput = 0.f;
			m_EveryDayNeedsOutput = 0.f;

			m_LuxuryGoodsInput = 0.f;
			m_LuxuryGoodsOutput = 0.f;
		}
	};

	struct HeroPopEffect
	{
		float upgrade_rate = 0.35f;
		float boost_percent = 0.f;
		float productBonus = 0.f;
		void reset()
		{
			upgrade_rate = 0.35f;
			boost_percent = 0.f;
			productBonus = 0.f;
		}
	};

	class PTMTownGraphics
	{
	public:
		PTMTownGraphics(PTMTown * parent);
		void updateGraphics();
	private:
		PTMTown * m_parent;
		Sprite * m_townSprite;
		Sprite * m_flagSprite;
		LabelNew * m_label;
		LabelNew * m_grassion = nullptr;

		
	};
	class PTMTown :public PTMILogicTickable, public PTMPawn
	{
	
	//production Level aka normal buildings
	PTM_PROPERTY(AgriDevLevel, uint32_t, 1, "the Grassion of town")
	PTM_PROPERTY(IndustryLevel, uint32_t, 1, "the Grassion of town")
	PTM_PROPERTY(EcoDevLevel, uint32_t, 1, "the Grassion of town")
	PTM_PROPERTY(HouseHoldLevel, uint32_t, 5, "the Grassion of town")

	PTM_PROPERTY(AgriDevProgress, float, 0.f, "the Grassion of town")
	PTM_PROPERTY(IndustryDevProgress, float, 0.f, "the Grassion of town")
	PTM_PROPERTY(EcoDevProgress, float, 0.f, "the Grassion of town")
	PTM_PROPERTY(HouseHoldDevProgress, float, 0.f, "the Grassion of town")
	
	PTM_PROPERTY(MilDevLevel, uint32_t, 1, "the Grassion of town")
	
	
	//prop production and consume output level
	PTM_PROPERTY_WITH_CAPACITY(Food, float, 500, 500, "Food")
	PTM_PROPERTY_WITH_CAPACITY(EveryDayNeeds, float, 0, 100, "Food")
	PTM_PROPERTY_WITH_CAPACITY(LuxuryGoods, float, 0, 100, "Food")

	PTM_PROPERTY(FortLevel, uint32_t, 1, "the fort level of town")
	PTM_PROPERTY(Garrison, int, 1000, "the Grassion of town")
	PTM_PROPERTY(GarrisonLimitBase, int, 1000, "the Grassion of town")
	PTM_PROPERTY(ManPower, int, 1000, "the Grassion of town")
	PTM_PROPERTY(ManPowerLimitBase, int, 1000, "the Grassion of town")
	PTM_PROPERTY(Autonomous, int, 5, "the Grassion of town")
	PTM_PROPERTY(Unrest, int, 5, "the Grassion of town")

	PTM_PROPERTY(Keeper, PTMHero * , nullptr, "the keeper of town")
	public:
		PTMTown(PTMTile * placedTile);
		void updateGraphics();
		void setName(std::string name) { m_name = name; }
		std::string getName() { return m_name; }
		PTMNation * getOwner() {return m_owner;}
		int getGarrisonLimit();
		virtual void onMonthlyTick() override;
		virtual void onDailyTick() override;
		virtual void onWeeklyTick() override;
		PawnTile getPawnType() override;
		PTMTaxPack collectTax();//return the tax
		void investEco();
		void investMil();
		void buildArmy(PTMHero * hero);
		void addOccupyTile(PTMTile * tile);
		void initPops();
		PTMPop * getPopAt(size_t index);
		size_t getTotalPopsNum();
		void tickPops();

		
		const PTMPopOutputView& getPopOutputView() {return m_popOutputView;}
		size_t getTotalOnDutyHeroes(DutyObjectiveEnum DutyObjective) {return m_onDutyHeroes[(int)DutyObjective].size();};
		PTMHero * getOnDutyHeroAt(int index, DutyObjectiveEnum DutyObjective);
		void assignOnDuty(PTMHero * hero, DutyObjectiveEnum DutyObjective);
		void kickOnDuty(PTMHero * hero);
		float getUpKeep(DutyObjectiveEnum DutyObjective);
		int town_x = -1, town_y = -1;
	private:
		void tickHeroDuty();
		PTMModifierContainer m_heroModContainer;
		std::vector<PTMTile *> m_occupyTiles;
		std::string m_name;
		PTMNation * m_occupant = nullptr;
		PTMNation * m_owner = nullptr;
		PTMTownGraphics * m_graphics;
		friend class PTMTownGraphics;
		friend class PTMNation;
		friend class PTMPawnJudge;
		PTMTaxPack m_taxPack;
		std::vector<PTMPop> m_pops;
		PTMPopOutputView m_popOutputView;
		std::vector<PTMHero * > m_onDutyHeroes[(int)DutyObjectiveEnum::MAX_NUM_DUTITES];
		PTMFiveElement m_heroesFiveElement;
		HeroPopEffect m_heroPopEffect;
	};

}
