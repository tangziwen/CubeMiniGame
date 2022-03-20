#include "PTMInspectorGUI.h"

#include <array>

#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMTile.h"
#include "PTMNation.h"
#include "2D/LabelNew.h"
#include "2D/GUIFrame.h"
#include "PTMArmyGUI.h"
#include "PTMWorld.h"
#include "PTMEventMgr.h"
#include "PTMTown.h"
#include "PTMPawnJudge.h"
#include "2D/imgui_internal.h"
#include <sstream>
#include "PTMConfig.h"

#include "PTMHero.h"
#include "PTMTech.h"
#include "PTMDepartment.h"
#define BORDER_LEFT 0
#define BORDER_RIGHT 1
#define BORDER_BOTTOM 2
#define BORDER_TOP 3

#define DRAW_PROPERTY(OBJ, PROP)\
{\
std::stringstream ss;\
ss<<OBJ->get##PROP();\
ImGui::Text("%s: ", OBJ->get##PROP##String());\
ImGui::SameLine();\
ImGui::SmallButton(ss.str().c_str());\
if(ImGui::IsItemHovered()&& GImGui->HoveredIdTimer > 0.3)\
{\
	ImGui::SetTooltip(OBJ->get##PROP##DescString());\
}\
}

#define DRAW_PROPERTY_LIMIT(OBJ, PROP)\
{\
std::stringstream ss;\
ss<<OBJ->get##PROP() << "/" << OBJ->get##PROP##Limit();\
ImGui::Text("%s: %s", OBJ->get##PROP##String(), ss.str());\
}

#define BEGIN_INSPECT(OBJ, WINDOW_TITLE) if(OBJ)\
{ bool isOpen = true; ImGui::Begin(WINDOW_TITLE, &isOpen);

#define END_INSPECT(OBJ)\
ImGui::End();\
if(!isOpen)\
{\
	OBJ = nullptr;\
}\
}



namespace tzw
{
	static void DrawNationTitle(PTMNation * nation)
	{
		if(nation)
		{
			//Draw flag color
			const ImVec2 p = ImGui::GetCursorScreenPos();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			float x = p.x + 4.0f;
			float y = p.y + 4.0f;
			float sz = 16;
			vec3 color = nation->getNationColor();
			const ImU32 col = ImColor(color.x, color.y, color.z);
			draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 0.0f, 0);
			ImGui::SetCursorScreenPos(ImVec2(x + sz, p.y));
			ImGui::Text(" Owner: %s ", nation->getName().c_str());
			ImGui::SameLine();
			if(ImGui::Button("*##gotoNation"))
			{
				PTMInspectorGUI::shared()->setInspectNation(nation);
			}
		}
		else
		{
			ImGui::Text(" Owner: unoccupy");
		}
	}

	static bool DrawButtonWithTips(const char * btnstr, const char * tips, bool condition)
	{
		if(!condition)
		{
			ImGuiContext& g = *GImGui;
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, g.Style.Alpha * 0.6f);
		}
		bool isClicked = ImGui::Button(btnstr);
		if(tips && ImGui::IsItemHovered()&& GImGui->HoveredIdTimer > 0.3)
		{
			ImGui::SetTooltip(tips);
		}
		if(!condition)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}
		return isClicked;
	}

	PTMInspectorGUI::PTMInspectorGUI()
	{
		
	}

	void PTMInspectorGUI::init()
	{
		GUISystem::shared()->addObject(this);
	}

	void PTMInspectorGUI::drawIMGUI()
	{
		drawEvents();
		drawNation();
		drawArmy();
		drawDepartmentConstruct();
		BEGIN_INSPECT(m_currInspectTown, "Town")
			PTMTown * t = m_currInspectTown;
			bool isControlledByPlayer = PTMWorld::shared()->getPlayerController()->getControlledNation() == t->getOwner();
			ImGui::Text("Name: %s ", t->getName().c_str());
				ImGui::SameLine();
				DrawNationTitle(t->getOwner());
			ImGui::Text("Keeper:");ImGui::SameLine();


			ImGui::BeginGroupPanel("Production");


			DRAW_PROPERTY(t, IndustryLevel)
				ImGui::SameLine();
				ImGui::ProgressBar(t->getIndustryDevProgress(), ImVec2(100, 0));

			DRAW_PROPERTY(t, HouseHoldLevel)
				ImGui::SameLine();
				ImGui::ProgressBar(t->getHouseHoldDevProgress(), ImVec2(100, 0));


			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel("Military");
				DRAW_PROPERTY(t, MilDevLevel)
					ImGui::SameLine();
					
					if(!isControlledByPlayer)
					{
						ImGuiContext& g = *GImGui;
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::PushStyleVar(ImGuiStyleVar_Alpha, g.Style.Alpha * 0.6f);
					}
					if(DrawButtonWithTips("Build Army", "Build An Army", true))
					{
						setInspectHeroList(t->getOwner(), [t](PTMHero * hero){t->buildArmy(hero);});
					}
					if(!isControlledByPlayer)
					{
						ImGui::PopItemFlag();
						ImGui::PopStyleVar();
					}
				DRAW_PROPERTY(t, Unrest)
				DRAW_PROPERTY_LIMIT(t, Garrison)
				DRAW_PROPERTY(t, ManPower);
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel("Pops");
			size_t popsTotal = t->getTotalPopsNum();

			for(int i = 0; i < popsTotal; i ++)
			{
				PTMPop * pop = t->getPopAt(i);
				ImGui::PushID(pop);
				char buff[128];
				sprintf(buff, "%s %s",pop->m_race->getName().c_str(), pop->m_job->getName().c_str());
				ImGui::SmallButton(buff);
				ImGui::SameLine();
				ImGui::Text("Lv: %d", pop->m_happinessLevel);
				ImGui::SameLine();
				ImGui::ProgressBar(pop->m_happiness, ImVec2(100, 0));
				ImGui::PopID();
			}
			ImGui::EndGroupPanel();
		END_INSPECT(m_currInspectTown)

		drawHeroDetail();
	}

	void PTMInspectorGUI::setInspectTown(PTMTown* town)
	{
		m_currInspectTown = town;
	}

	void PTMInspectorGUI::setInspectNation(PTMNation* nation)
	{
		m_currInspectNation = nation;
	}

	void PTMInspectorGUI::setInspectTownList(PTMNation* nation, std::function<void(PTMTown*)> clickCB)
	{
		m_currInspectTownListNation = nation;
		m_townListClickedCB = clickCB;
	}

	void PTMInspectorGUI::setInspectHeroList(PTMNation* nation, std::function<void(PTMHero*)> clickCB)
	{
		m_currInspectHeroListNation = nation;
		m_heroesListClickedCB = clickCB;
	}

	void PTMInspectorGUI::setInspectHero(PTMHero* hero)
	{
		m_currInspectHero = hero;
	}

	void PTMInspectorGUI::setInspectArmy(PTMArmy* army)
	{
		m_currInspectArmy = army;
	}

	void PTMInspectorGUI::setInspectDepartmentBuilding(PTMDepartment* m_department)
	{
		m_currInspectDepartmentConstruct = m_department;
	}

	void PTMInspectorGUI::drawNation()
	{

		if(m_currInspectTownListNation)
		{
			bool isOpen = true;
			ImGui::Begin("Town List", &isOpen,ImGuiWindowFlags_NoResize);
			ImVec2 winSize(300, 500);
			ImGui::SetWindowSize("Town List", winSize);
			PTMNation * t = m_currInspectTownListNation;
			auto townList = t->getTownList();
			for(PTMTown * town : townList)
			{
				ImGui::Text("%s", town->getName().c_str());
					ImGui::SameLine();
					ImGui::PushID(town);
					if(ImGui::Button("Select ##gototown"))
					{
						m_currInspectTownListNation = nullptr;
						m_townListClickedCB(town);
					}
					ImGui::PopID();
			}
			if(!isOpen)
			{
				m_currInspectTownListNation = false;
			}
		}


		if(m_currInspectHeroListNation)
		{
			bool isOpen = true;
			ImGui::Begin("Heroes List", &isOpen,ImGuiWindowFlags_NoResize);
			ImVec2 winSize(300, 500);
			ImGui::SetWindowSize("Heroes List", winSize);
			PTMNation * t = m_currInspectHeroListNation;
			size_t heroSize = t->getTotalHerosNum();
			for(int i = 0; i < heroSize; i++)
			{
				PTMHero * hero = t->getHeroAt(i);
				drawHeroSmall(hero, false);
				ImGui::SameLine();
				ImGui::PushID(hero);
				if(ImGui::Button("Select ##gototown"))
				{
					m_currInspectHeroListNation = nullptr;
					m_heroesListClickedCB(hero);
				}
				ImGui::PopID();
			}
			if(!isOpen)
			{
				m_currInspectHeroListNation = false;
			}
		}

		BEGIN_INSPECT(m_currInspectNation, "Nation")
		PTMNation * t = m_currInspectNation;
		ImGui::BeginTabBar("NationTabs");
		if(ImGui::BeginTabItem("Basic"))
		{
				
			ImGui::Text("Name: %s ", t->getName().c_str());
			auto townList = t->getTownList();
			PTMCurrencyPool *pool =  t->getNationalCurrency();

			for(int i = 0; i < (int)PTMCurrencyEnum::CurrencyMax; i++)
			{
				ImGui::PushID(i);
				ImGui::Text("%s:%f", getCurrencyStr((PTMCurrencyEnum)i), pool->get((PTMCurrencyEnum)i));
				ImGui::PopID();
			}
			ImGui::Text("Own Provinces: %u ", townList.size());
			
			ImGui::BeginChild("Pronvices List:",ImVec2(0,0), true);
			for(PTMTown * town : townList)
			{
				ImGui::Text("%s", town->getName().c_str());
					ImGui::SameLine();
					ImGui::PushID(town);
					if(ImGui::Button("*##gototown"))
					{
						
						m_currInspectTown = town;
					}
					ImGui::PopID();
			}
			ImGui::EndChild();
			ImGui::Text("Global Modifier");
			ImGui::EndTabItem();
		}


		if(ImGui::BeginTabItem("Departments"))
		{
		
			ImGui::BeginTabBar("Departments##innerTabar");
			const std::vector<PTMDepartment*> & departments = t->getDepartments();

			for(PTMDepartment * deparment : departments)
			{
				ImGui::PushID(deparment);
				if(ImGui::BeginTabItem(deparment->getName().c_str()))
				{
					ImGui::Text("Total Size %d/%d",deparment->getHeroList().size(), deparment->getTotalSlotSize());
					ImGui::SameLine();
					if(deparment->getIsWorking())
					{
						ImGui::TextColored(ImVec4(0.f, 1.0f, 0.f, 1.0f), "Working");
					}
					else
					{
						ImGui::TextColored(ImVec4(1.f, 0.0f, 0.f, 1.0f), "Lack of Resource");
					}
					if(deparment->getCurrBuildingTarget())
					{
						ImGui::Text("building :%s ", deparment->getCurrBuildingTarget()->Title.c_str());
						ImGui::ProgressBar(deparment->getCurrBuildingPercentage());
					}
					//input
					ImGui::BeginGroupPanel("Input");
					PTMDepartMentInputOutput * input =  deparment->getInput();
					for(auto &iter: input->m_val)
					{
						ImGui::PushID((int)iter.first);
						ImGui::Text("%s : %f", getCurrencyStr(iter.first), iter.second);
						ImGui::PopID();
					}
					ImGui::EndGroupPanel();


					ImGui::SameLine();

					//output
					ImGui::BeginGroupPanel("Output");
					PTMDepartMentInputOutput * output =  deparment->getOutput();
					for(auto &iter: output->m_val)
					{
						ImGui::PushID((int)iter.first);
						ImGui::Text("%s : %f", getCurrencyStr(iter.first), iter.second);
						ImGui::PopID();
					}
					ImGui::EndGroupPanel();

					ImGui::SameLine();

					//Building
					ImGui::BeginGroupPanel("Building");
					if(ImGui::SmallButton("+"))
					{
						setInspectDepartmentBuilding(deparment);
					}
					auto & buildings =  deparment->getBuildings();
					for(PTMBuilding* building: buildings)
					{
						ImGui::PushID(building);
						ImGui::Text("%s", building->data->Title.c_str());
						ImGui::PopID();
					}
					ImGui::EndGroupPanel();
					
					auto & heroes = deparment->getHeroList();
					for(PTMHero * hero : heroes)
					{
						drawHeroSmall(hero, false);
					}
					ImGui::EndTabItem();
				}
				ImGui::PopID();
			}
			ImGui::EndTabBar();

			ImGui::EndTabItem();
		}
		
		
		if(ImGui::BeginTabItem("Technology"))
		{
			ImGui::Text("Tech Level");
			ImGui::BeginTabBar("TechStages");
			PTMTechState * techState = t->getTechState();
			PTMTech* tech =  techState->getTech();
			for(int i = 0; i< tech->getTotalStage(); i++)
			{
				PTMTechStage * stage = tech->getStage(i);
				if(ImGui::BeginTabItem(stage->m_name.c_str()))
				{
					int perkID = 0;
					for(auto& perk : stage->m_perks)
					{
						ImGui::BeginGroup();

						bool isFinished = techState->isFinished(i, perkID);
						bool isClicked = DrawButtonWithTips(perk.m_title.c_str(), nullptr, i <= techState->getCurrentFocusLevel() && !isFinished);
						if(isClicked)
						{
							techState->setCurrentFocusIndex(perkID);
						}
						if(isFinished)
						{
							ImVec2 sizeMin = ImGui::GetItemRectMin();
							ImVec2 sizeMax = ImGui::GetItemRectMax();
							GUISystem::shared()->imgui_drawFrame(sizeMin, sizeMax, 3.0, ImVec4(0.f, 1.f, 0.f, 1.f));
						}
						else if(i == techState->getCurrentFocusLevel() && perkID == techState->getCurrentFocusIndex())
						{
							ImVec2 sizeMin = ImGui::GetItemRectMin();
							ImVec2 sizeMax = ImGui::GetItemRectMax();
							GUISystem::shared()->imgui_drawFrame(sizeMin, sizeMax, 3.0, ImVec4(1.f, 1.f, 0.f, 1.f));
						}
						ImGui::EndGroup();
						ImGui::SameLine();
						ImGui::ProgressBar(techState->getProgress(i,perkID));
						perkID++;
					}
					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
			ImGui::EndTabItem();
		}

		if(ImGui::BeginTabItem("Heroes"))
		{
			size_t num = t->getTotalHerosNum();
			for(int i = 0; i < num; i++)
			{
				PTMHero * hero = t->getHeroAt(i);
				drawHeroSmall(hero, true);
			}
			ImGui::EndTabItem();
		}
		END_INSPECT(m_currInspectNation)
	}

	void PTMInspectorGUI::drawEvents()
	{
		PTMNation * nation = PTMWorld::shared()->getPlayerController()->getControlledNation();

		auto& eventList = nation->getEventInstancedList();
		PTMInGameEvent* removeEvent {nullptr};
		for(auto&inGameEventIter : eventList)
		{
			PTMInGameEventInstanced& eventInstanced = inGameEventIter.second;
			PTMInGameEvent * inGameEvent = eventInstanced.m_parent;
			ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
			ImGui::Begin(inGameEvent->m_title.c_str());
			ImGui::Text(inGameEvent->m_desc.c_str());
			for(auto& option:inGameEvent->m_options)
			{
				if(ImGui::Button(option.m_Title.c_str()))
				{
					option.trigger(nation);
					removeEvent = inGameEventIter.first;
				}
			}
			ImGui::End();
		}

		if(removeEvent)
		{
			eventList.erase(removeEvent);
		}
		
	}

	void PTMInspectorGUI::drawHeroSmall(PTMHero* hero, bool isShowLocation)
	{
		ImGui::PushID(hero);
		if(ImGui::Button(hero->getName().c_str(), ImVec2(80, 0)))
		{
			setInspectHero(hero);
		}
		ImGui::SameLine();
		ImGui::Text("Lv:%d ETM: %d", hero->getLevel(), (int)std::ceil(hero->getEstimateLevelUpMonth()));
		ImGui::SameLine();
		const PTMFiveElement & fe = hero->getFiveElement();
		ImGui::Text(u8"����:");
		//, fe.ElementMetal, fe.ElementWood, fe.ElementWater, fe.ElementFire, fe.ElementEarth);
		ImGui::SameLine();ImGui::TextColored(ImVec4(1.0f, 0.84f, 0.f, 1.f), "%d", fe.ElementMetal);
		ImGui::SameLine();ImGui::TextColored(ImVec4(0.1f, 0.9f, 0.1f, 1.f), "%d", fe.ElementWood);
		ImGui::SameLine();ImGui::TextColored(ImVec4(0.1f, 0.9f, 0.9f, 1.f), "%d", fe.ElementWater);
		ImGui::SameLine();ImGui::TextColored(ImVec4(0.9f, 0.1f, 0.1f, 1.f), "%d", fe.ElementFire);
		ImGui::SameLine();ImGui::TextColored(ImVec4(212/255.f, 148 / 255.f, 32/255.f, 1.f), "%d", fe.ElementEarth);
		ImGui::PopID();
	}

	void PTMInspectorGUI::drawHeroDetail()
	{
		BEGIN_INSPECT(m_currInspectHero, "Hero Detail")
			PTMHero * hero = m_currInspectHero;


			std::string role = u8"��ְλ";
			switch(hero->getCurrRole())
			{
			case PTMHeroRole::Idle:
				break;
			case PTMHeroRole::Keeper:
				role = hero->getTownOfKeeper()->getName() + u8"����";
				break;
			case PTMHeroRole::OnDuty:
				role = hero->getTownOfOnDuty()->getName() + u8"��ͨ��ְ";
				break;
			case PTMHeroRole::Admin:
				role = u8"����";
				break;
			case PTMHeroRole::Eco:
				role = u8"����";
				break;
			case PTMHeroRole::Mil:
				role = u8"����";
				break;
			}
			ImGui::Text(u8" ְλ:%s", role.c_str());
			const PTMFiveElement & fe = hero->getFiveElement();
			ImGui::Text(u8"����:");
			//, fe.ElementMetal, fe.ElementWood, fe.ElementWater, fe.ElementFire, fe.ElementEarth);
			ImGui::SameLine();ImGui::TextColored(ImVec4(1.0f, 0.84f, 0.f, 1.f), "%d", fe.ElementMetal);
			ImGui::SameLine();ImGui::TextColored(ImVec4(0.1f, 0.9f, 0.1f, 1.f), "%d", fe.ElementWood);
			ImGui::SameLine();ImGui::TextColored(ImVec4(0.1f, 0.9f, 0.9f, 1.f), "%d", fe.ElementWater);
			ImGui::SameLine();ImGui::TextColored(ImVec4(0.9f, 0.1f, 0.1f, 1.f), "%d", fe.ElementFire);
			ImGui::SameLine();ImGui::TextColored(ImVec4(212/255.f, 148 / 255.f, 32/255.f, 1.f), "%d", fe.ElementEarth);


			//central
			ImGui::Text(u8"���ŵ���");
			if(ImGui::Button("Farming"))
			{
				hero->getCountry()->addHeroToDepartment(NATION_DEPARTMENT_FARMING, hero);
			}
			ImGui::SameLine();
			if(ImGui::Button("ALCHEMY"))
			{
				hero->getCountry()->addHeroToDepartment(NATION_DEPARTMENT_ALCHEMY, hero);
			}

			if(ImGui::Button("Idle"))
			{
				hero->getCountry()->addHeroToDepartment(NATION_DEPARTMENT_IDLE, hero);
			}
			ImGui::SameLine();
			if(ImGui::Button("Research"))
			{
				hero->getCountry()->assignResearchHero(hero);
			}


		END_INSPECT(m_currInspectHero)
	}

	void PTMInspectorGUI::drawArmy()
	{
		BEGIN_INSPECT(m_currInspectArmy, "Army Detail")
			auto & heroList = m_currInspectArmy->getHeroList();
			ImGui::Text(u8"%s ��", heroList[0]->getName().c_str());
			if(ImGui::SmallButton("+"))
			{
				setInspectHeroList(PTMWorld::shared()->getPlayerController()->getControlledNation(), 
					[this](PTMHero * hero)
					{
						m_currInspectArmy->addHero(hero);
					});
			}
		END_INSPECT(m_currInspectArmy)
	}

	void PTMInspectorGUI::drawDuty(PTMTown * t, std::string title, int dutyEnum)
	{
		DutyObjectiveEnum objective = (DutyObjectiveEnum)dutyEnum;

		{
			ImGui::PushID(title.c_str());
			ImGui::BeginGroupPanel(title.c_str());
			size_t num = t->getTotalOnDutyHeroes(objective);
			ImGui::Text("HeroesList");ImGui::SameLine();
			if(ImGui::SmallButton("+##assignMoreHero"))
			{
				setInspectHeroList(t->getOwner(), [t, objective](PTMHero * hero){t->getOwner()->assignOnDuty(t, hero, (int)objective);});
			}
			ImGui::SameLine();ImGui::Text("UpKeep %f", t->getUpKeep(objective));
			if(num)
			{
				ImGui::BeginChild("HeroesList", ImVec2(-1, 200));
				{
					for(int i = 0; i < num; i++)
					{
						PTMHero * hero = t->getOnDutyHeroAt(i, objective);
						drawHeroSmall(hero, false);
						ImGui::PushID(hero);
						ImGui::SameLine();
						ImGui::ProgressBar(hero->getDutyProgress() / (hero->getDutyProgressMax() * 1.0f));
						ImGui::PopID();
					}
				}
				ImGui::EndChild();
			}
			ImGui::EndGroupPanel();
			ImGui::PopID();
		}
	}

	void PTMInspectorGUI::drawDepartmentConstruct()
	{
		BEGIN_INSPECT(m_currInspectDepartmentConstruct, "Department Construct")
			auto & buildingDataList = m_currInspectDepartmentConstruct->getBuildingDataList();
			for(PTMBuildingData * data : buildingDataList)
			{
				ImGui::PushID(data);
				ImGui::Text("%s", data->Title.c_str());
				ImGui::SameLine();
				if(ImGui::SmallButton("build"))
				{
					m_currInspectDepartmentConstruct->constructBuilding(data);
					m_currInspectDepartmentConstruct = nullptr;
				}
				ImGui::PopID();
			}
		END_INSPECT(m_currInspectDepartmentConstruct)
	}

	const char* PTMInspectorGUI::getCurrencyStr(PTMCurrencyEnum currency)
	{
		switch (currency)
		{
		case tzw::PTMCurrencyEnum::Gold:
			return "Gold";
			break;
		case tzw::PTMCurrencyEnum::Herb:
			return "Herb";
			break;
		case tzw::PTMCurrencyEnum::Minerals:
			return "Minerals";
			break;
		case tzw::PTMCurrencyEnum::Dan:
			return "Dan";
			break;
		default:
			break;
		}
		return "NotValid";
	}

}