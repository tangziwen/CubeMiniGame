#pragma once
#include "PTMPawn.h"
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
#include "PTMBaseDef.h"
namespace tzw
{
	class PTMNation;

	struct PTMTechPerk
	{
		int m_id = 0;
		std::string m_Name;
		std::string m_title;
		std::unordered_map<std::string, float> m_effect;
	};

	struct PTMTechStage
	{
		std::string m_name;
		std::vector<PTMTechPerk> m_perks;
	};
	class PTMTech
	{
	public:
		PTMTech();
		void loadFromFile(std::string filePath);
		PTMTechStage* getStage(int stage);
		int getTotalStage();
	protected:
		std::vector<PTMTechStage * > m_stages;
	};

	class PTMTechState
	{
	PTM_PROPERTY(CurrentLevel,int, 0, "the CurrentLevel")
	PTM_PROPERTY(CurrentProgress,int, 0, "Current Progress")
	PTM_PROPERTY(CurrentFocusIndex,int, -1, "Current Progress")
	PTM_PROPERTY(CurrentFocusLevel,int, -1, "Current Progress")
	public:
		PTMTechState(PTMNation * nation, PTMTech * tech);
		PTMTech * getTech(){return m_tech;};
		void doProgress(float progressRate);
		float getProgress(int level, int perkID);
		bool isFinished(int level, int perkID);
	protected:
		PTMTech * m_tech {nullptr};
		PTMNation * m_parent {nullptr};
		std::unordered_map<int, std::unordered_map<int, float>> m_techPerkMap{};
		std::unordered_map<int, std::unordered_set<int>> m_techPerkFinished{};
	};


	class PTMTechMgr: public Singleton<PTMTechMgr>
	{
	public:
		PTMTechMgr();
		PTMTechState * generateTechState(PTMNation * nation, std::string filePath);
	protected:
		std::unordered_map<std::string, PTMTech*> m_techList;
	
	};
}