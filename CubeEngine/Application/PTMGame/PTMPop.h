#pragma once
#include "PTMPawn.h"
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
#include "2D/TileMap2DMgr.h"
#include "PTMBaseDef.h"


namespace tzw
{
	constexpr int POP_MIN_HAPPY_LEVEL = 0;
	constexpr int  POP_MAX_HAPPY_LEVEL  = 5;

	class PTMPopRaceType
	{
	public:
		PTM_PROPERTY(Name, std::string, 1, "the Grassion of town")
		PTM_PROPERTY(FoodConsume, float, 1.0f, "the Grassion of town")
		PTM_PROPERTY(EveryDayNeedsConsume, float, 0.3f, "the Grassion of town")
		PTM_PROPERTY(LuxuryGoodsConsume, float, 0.1f, "the Grassion of town")
	};


	class PTMPopJobType
	{

		PTM_PROPERTY(Name, std::string, 1, "the Grassion of town")
		//self need
		PTM_PROPERTY(FoodProduct, float, -1.f, "the Grassion of town")
		PTM_PROPERTY(EveryDayNeedsProduct, float, -1.f, "the Grassion of town")
		PTM_PROPERTY(LuxuryGoodsProduct, float, -1.f, "the Grassion of town")

		//National Output (player reward)
		PTM_PROPERTY(GoldProduct, float, -1.f, "the Grassion of town")
		PTM_PROPERTY(AdmProduct, float, -1.f, "the Grassion of town")
	public:
		int m_JobType = 0;
	};

	struct PTMPop
	{
		PTMPopRaceType * m_race = nullptr;
		PTMPopJobType * m_job = nullptr;
		float m_happiness = 0.5f;
		int m_happinessLevel = 0;// 0 riot 1 unhappy 2 normal 3 relative happy 4 happy 5 super happy

		void selfUpgradeMaybe();
	};

	class PTMPopFactory : public Singleton<PTMPopFactory>
	{
	public:
		void init();
		void loadRaces();
		void loadJobs();
		void createAPop(PTMPop * pop, int jobType);//pop的数量可能会非常的多，这里只加工和初始化，本身不负责分配
	private:
		std::vector<PTMPopRaceType> m_races;
		std::vector<PTMPopJobType> m_jobs;
	};

}