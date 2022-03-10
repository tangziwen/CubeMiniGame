#pragma once
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
#include "PTMILogicTickable.h"
#include "PTMPawn.h"
#include "PTMBaseDef.h"
namespace tzw
{
	class LabelNew;
	struct PTMTile;
	class PTMTown;
	class PTMNation;
	class PTMArmy;
	class GUIFrame;
	class Button;
	class PTMHero;
	class PTMArmyGraphics
	{
	public:
		PTMArmyGraphics(PTMArmy * parent);
		~PTMArmyGraphics();
		void updateGraphics();
	private:
		PTMArmy * m_parent = nullptr;
		Sprite * m_sprite = nullptr;
		Sprite * m_flagSprite = nullptr;
		LabelNew * m_label = nullptr;
		GUIFrame * m_selectedBorders[4] {nullptr};
		Button * m_button {nullptr};
		
	};

	enum class PTMArmyRole
	{
		Hunting,
		Aggressive,
	};

	class PTMArmy : public PTMILogicTickable, public PTMPawn
	{
	PTM_PROPERTY(ArmyRole, PTMArmyRole, PTMArmyRole::Hunting, "ArmyRole")
	public:
		PTMArmy(PTMNation * nation, PTMHero * hero, PTMTile * targetTile);
		virtual ~PTMArmy();
		void updateGraphics();
		void setTile(PTMTile * targetTile) override;
		
		virtual void onMonthlyTick() override;
		virtual void onDailyTick() override;
		virtual void onWeeklyTick() override;

		void setIsSelected(bool isSelect);//only for player
		void moveTo(PTMTile * tile);
		void simulateMilitary();
		PawnTile getPawnType() override;
		std::vector<PTMHero * > & getHeroList();
		void addHero(PTMHero * hero);
	private:
		uint32_t m_currSize = 0;
		uint32_t m_sizeLimit = 0;
		PTMArmyGraphics * m_graphics = nullptr;
		PTMNation * m_parent = nullptr;
		PTMTile * m_targetTile = nullptr;
		std::vector<PTMHero * > m_herolist;
		bool m_isSelected = false;
		friend class PTMArmyGraphics;
		friend class PTMPawnJudge;
	};

}