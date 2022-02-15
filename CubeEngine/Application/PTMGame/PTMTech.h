#pragma once
#include "PTMPawn.h"
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
#include "PTMBaseDef.h"
namespace tzw
{
	class PTMNation;
	class PTMTech
	{
	public:
		PTMTech();
		void loadFromFile(std::string filePath);
	};

	class PTMTechState
	{
	PTM_PROPERTY(CurrentLevel,int, 0, "the CurrentLevel")
	PTM_PROPERTY(CurrentProgress,int, 0, "Current Progress")
	public:
		PTMTechState(PTMNation * nation, PTMTech * tech);
		PTMTech * getTech(){return m_tech;};
		void doProgress(int progressRate);
	protected:
		PTMTech * m_tech {nullptr};
		PTMNation * m_parent {nullptr};
	};
}