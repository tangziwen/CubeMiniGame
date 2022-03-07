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

#include "PTMHero.h"
#include "PTMTech.h"
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
		
		BEGIN_INSPECT(m_currInspectTown, "Town")
			PTMTown * t = m_currInspectTown;
			bool isControlledByPlayer = PTMWorld::shared()->getPlayerController()->getControlledNation() == t->getOwner();
			ImGui::Text("Name: %s ", t->getName().c_str());
				ImGui::SameLine();
				DrawNationTitle(t->getOwner());
			ImGui::Text("Keeper:");ImGui::SameLine();
			if(t->getKeeper())
			{
				ImGui::SmallButton(t->getKeeper()->getName().c_str());
			}
			else
			{
				ImGui::SmallButton("None");
			}
			ImGui::BeginChild("HeroesList", ImVec2(-1, 200));
			{
				size_t num = t->getTotalOnDutyHeroes();
				for(int i = 0; i < num; i++)
				{
					PTMHero * hero = t->getOnDutyHeroAt(i);
					drawHeroSmall(hero, false);
			        const char* items[] = { u8"训练", u8"开发", u8"工作"};
			        int item_current_idx = (int)hero->getDutyObjective(); // Here we store our selection data as an index.
			        const char* combo_preview_value = items[item_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)
			        if (ImGui::BeginCombo("##combo", combo_preview_value, 0))
			        {
			            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			            {
			                const bool is_selected = (item_current_idx == n);
			                if (ImGui::Selectable(items[n], is_selected))
			                    item_current_idx = n;

			                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			                if (is_selected)
			                    ImGui::SetItemDefaultFocus();
			            }
			            ImGui::EndCombo();
			        }
					hero->setDutyObjective((DutyObjectiveEnum)item_current_idx);
				}
			}

			ImGui::EndChild();

			ImGui::BeginGroupPanel("Production");
			DRAW_PROPERTY(t, AgriDevLevel)
					ImGui::SameLine();
					ImGui::ProgressBar(t->getAgriDevProgress(), ImVec2(100, 0));

			DRAW_PROPERTY(t, IndustryLevel)
				ImGui::SameLine();
				ImGui::ProgressBar(t->getIndustryDevProgress(), ImVec2(100, 0));

			DRAW_PROPERTY(t, EcoDevLevel)
				ImGui::SameLine();
				ImGui::ProgressBar(t->getEcoDevProgress(), ImVec2(100, 0));

			DRAW_PROPERTY(t, HouseHoldLevel)
				ImGui::SameLine();
				ImGui::ProgressBar(t->getHouseHoldDevProgress(), ImVec2(100, 0));

			DRAW_PROPERTY(t, Autonomous)

			DRAW_PROPERTY(t, Food)
			DRAW_PROPERTY(t, EveryDayNeeds)
			DRAW_PROPERTY(t, LuxuryGoods)
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
					ImGui::Button("Boost Mil");

					DrawButtonWithTips("Build Army", "Build An Army", true);
					DrawButtonWithTips("Suprress Unrest", "Build An Army", true);
					if(!isControlledByPlayer)
					{
						ImGui::PopItemFlag();
						ImGui::PopStyleVar();
					}
				DRAW_PROPERTY(t, Unrest)
				DRAW_PROPERTY_LIMIT(t, Garrison)
				DRAW_PROPERTY(t, ManPower);
			ImGui::EndGroupPanel();
			ImGui::BeginGroupPanel("Politics");
			DrawButtonWithTips("Issue Edict", "Build An Army", true);
			ImGui::EndGroupPanel();
			ImGui::BeginGroupPanel("Pops");
			size_t popsTotal = t->getTotalPopsNum();
			const PTMPopOutputView& outputView = t->getPopOutputView();
			ImGui::Text(u8"消费: 食物: %.2f, 日用品: %.2f, 奢侈品: %.2f", outputView.m_FoodInput, outputView.m_EveryDayNeedsInput, outputView.m_FoodInput);
			ImGui::Text(u8"生产: 食物: %.2f, 日用品: %.2f, 奢侈品: %.2f", outputView.m_FoodOutput, outputView.m_EveryDayNeedsOutput, outputView.m_LuxuryGoodsOutput);
			ImGui::Text(u8"盈余: 食物: %.2f, 日用品: %.2f, 奢侈品: %.2f", outputView.m_FoodOutput - outputView.m_FoodInput, 
				outputView.m_EveryDayNeedsOutput - outputView.m_EveryDayNeedsInput, outputView.m_LuxuryGoodsOutput - outputView.m_LuxuryGoodsInput);
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

	void PTMInspectorGUI::setInspectHero(PTMHero* hero)
	{
		m_currInspectHero = hero;
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

		BEGIN_INSPECT(m_currInspectNation, "Nation")
		PTMNation * t = m_currInspectNation;
		ImGui::BeginTabBar("NationTabs");
		if(ImGui::BeginTabItem("Basic"))
		{
				
			ImGui::Text("Name: %s ", t->getName().c_str());
			DRAW_PROPERTY(t, AdminPoint)
			DRAW_PROPERTY(t, MilitaryPoint)
			DRAW_PROPERTY(t, GlobalManPower)
			auto townList = t->getTownList();
			ImGui::Text("Admin Heroes size %d", t->getAdminHeroes().size());
			ImGui::Text("Eco Heroes size %d", t->getEcoHeroes().size());
			ImGui::Text("Research Heroes size %d", t->getResearchHeroes().size());
			ImGui::Text("Mil Heroes size %d", t->getMilHeroes().size());

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
			
		if(ImGui::BeginTabItem("Admin"))
		{
			ImGui::Text("Tech Level");

			auto & heroes = t->getAdminHeroes();
			for(PTMHero * hero : heroes)
			{
				drawHeroSmall(hero, false);
			}
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
		if(ImGui::SmallButton(hero->getName().c_str()))
		{
			setInspectHero(hero);
		}

		std::string role = u8"无职位";
		switch(hero->getCurrRole())
		{
		case PTMHeroRole::Idle:
			break;
		case PTMHeroRole::Keeper:
			role = hero->getTownOfKeeper()->getName() + u8"城守";
			break;
		case PTMHeroRole::OnDuty:
			role = hero->getTownOfOnDuty()->getName() + u8"普通就职";
			break;
		}
		ImGui::SameLine(); ImGui::Text(u8"职位:%s", role.c_str());

		if(isShowLocation)
		{
			ImGui::SameLine(); ImGui::Text(u8"所在");ImGui::SameLine();
			if( ImGui::SmallButton(hero->getTownLocation()->getName().c_str()))
			{
				setInspectTown(hero->getTownLocation());
			}
		}
		const PTMFiveElement & fe = hero->getFiveElement();
		ImGui::Text(u8"能力:");
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


			std::string role = u8"无职位";
			switch(hero->getCurrRole())
			{
			case PTMHeroRole::Idle:
				break;
			case PTMHeroRole::Keeper:
				role = hero->getTownOfKeeper()->getName() + u8"城守";
				break;
			case PTMHeroRole::OnDuty:
				role = hero->getTownOfOnDuty()->getName() + u8"普通就职";
				break;
			case PTMHeroRole::Admin:
				role = u8"行政";
				break;
			case PTMHeroRole::Eco:
				role = u8"民生";
				break;
			case PTMHeroRole::Mil:
				role = u8"扩张";
				break;
			}
			ImGui::Text(u8"所在:%s, 职位:%s", hero->getTownLocation()->getName().c_str(), role.c_str());
			const PTMFiveElement & fe = hero->getFiveElement();
			ImGui::Text(u8"能力:");
			//, fe.ElementMetal, fe.ElementWood, fe.ElementWater, fe.ElementFire, fe.ElementEarth);
			ImGui::SameLine();ImGui::TextColored(ImVec4(1.0f, 0.84f, 0.f, 1.f), "%d", fe.ElementMetal);
			ImGui::SameLine();ImGui::TextColored(ImVec4(0.1f, 0.9f, 0.1f, 1.f), "%d", fe.ElementWood);
			ImGui::SameLine();ImGui::TextColored(ImVec4(0.1f, 0.9f, 0.9f, 1.f), "%d", fe.ElementWater);
			ImGui::SameLine();ImGui::TextColored(ImVec4(0.9f, 0.1f, 0.1f, 1.f), "%d", fe.ElementFire);
			ImGui::SameLine();ImGui::TextColored(ImVec4(212/255.f, 148 / 255.f, 32/255.f, 1.f), "%d", fe.ElementEarth);

			//local
			ImGui::Text(u8"地方调动");
			if(ImGui::Button("Set As Keeper"))
			{
				setInspectTownList(hero->getCountry(), [hero](PTMTown* town){hero->getCountry()->assignTownKeeper(town, hero);});
			}
			ImGui::SameLine();
			if(ImGui::Button("Assign On Duty"))
			{
				setInspectTownList(hero->getCountry(), [hero](PTMTown* town){hero->getCountry()->assignOnDuty(town, hero);});
			}

			//central
			ImGui::Text(u8"中央调动");
			if(ImGui::Button("Admin"))
			{
				hero->getCountry()->assignAdmHero(hero);
			}
			ImGui::SameLine();
			if(ImGui::Button("Eco"))
			{
				hero->getCountry()->assignEcoHero(hero);
			}

			if(ImGui::Button("Mil"))
			{
				hero->getCountry()->assignMilHero(hero);
			}
			ImGui::SameLine();
			if(ImGui::Button("Research"))
			{
				hero->getCountry()->assignResearchHero(hero);
			}


		END_INSPECT(m_currInspectHero)
	}

}