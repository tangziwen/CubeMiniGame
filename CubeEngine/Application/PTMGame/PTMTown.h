#pragma once
#include "Engine/EngineDef.h"
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

		
	};
	class PTMTown :public PTMILogicTickable, public PTMPawn
	{
	public:
		PTMTown(PTMTile * placedTile);
		void updateGraphics();
		void setName(std::string name) { m_name = name; }
		std::string getName() { return m_name; }
		uint32_t getEcoLevel() {return m_ecoDevLevel;}
		uint32_t getMilLevel() {return m_milDevLevel;}
		PTMNation * getOwner() {return m_owner;}
		int getGarrisonLimit();
		virtual void onMonthlyTick() override;
		virtual void onDailyTick() override;
		PawnTile getPawnType() override;
		float collectTax();//return the tax
		void investEco();
		void investMil();
		void buildArmy();
	private:
		uint32_t m_ecoDevLevel = 1;
		uint32_t m_milDevLevel = 1;
		int m_garrison = 100;
		int m_garrisonBaseLimit = 1000;
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