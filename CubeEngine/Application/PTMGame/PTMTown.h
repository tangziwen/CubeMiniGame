#pragma once
#include "Engine/EngineDef.h"
#include "PTMBaseDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
#include "PTMILogicTickable.h"
#include "PTMPawn.h"
namespace tzw
{
	class LabelNew;
	class Button;
	struct PTMTile;
	class PTMTown;
	class PTMNation;
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
	PTM_PROPERTY(EcoDevLevel, uint32_t, 1, "the Grassion of town")
	PTM_PROPERTY(MilDevLevel, uint32_t, 1, "the Grassion of town")
	PTM_PROPERTY(FortLevel, uint32_t, 1, "the fort level of town")
	PTM_PROPERTY(Garrison, int, 1000, "the Grassion of town")
	PTM_PROPERTY(GarrisonLimitBase, int, 1000, "the Grassion of town")
	PTM_PROPERTY(ManPower, int, 1000, "the Grassion of town")
	PTM_PROPERTY(ManPowerLimitBase, int, 1000, "the Grassion of town")
	PTM_PROPERTY(Autonomous, int, 5, "the Grassion of town")
	PTM_PROPERTY(Unrest, int, 5, "the Grassion of town")

	public:
		PTMTown(PTMTile * placedTile);
		void updateGraphics();
		void setName(std::string name) { m_name = name; }
		std::string getName() { return m_name; }
		PTMNation * getOwner() {return m_owner;}
		int getGarrisonLimit();
		virtual void onMonthlyTick() override;
		virtual void onDailyTick() override;
		PawnTile getPawnType() override;
		float collectTax();//return the tax
		void investEco();
		void investMil();
		void buildArmy();
		void addOccupyTile(PTMTile * tile);
	private:
		std::vector<PTMTile *> m_occupyTiles;
		std::string m_name;
		PTMNation * m_occupant = nullptr;
		PTMNation * m_owner = nullptr;
		PTMTownGraphics * m_graphics;
		friend class PTMTownGraphics;
		friend class PTMNation;
		friend class PTMPawnJudge;
		float m_taxAccum{};
	};

}