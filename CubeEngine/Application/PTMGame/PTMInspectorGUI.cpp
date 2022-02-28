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
					ImGui::SmallButton(hero->getName().c_str());
				}
			}

			ImGui::EndChild();

			ImGui::BeginGroupPanel("Production");
			DRAW_PROPERTY(t, AgriDevLevel)
					ImGui::SameLine();
					ImGui::Button("Boost Agri");

			DRAW_PROPERTY(t, EcoDevLevel)
				ImGui::SameLine();
				ImGui::Button("Boost Eco");

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
			ImGui::Text(u8"����: ʳ��: %.2f, ����Ʒ: %.2f, �ݳ�Ʒ: %.2f", outputView.m_FoodInput, outputView.m_EveryDayNeedsInput, outputView.m_FoodInput);
			ImGui::Text(u8"����: ʳ��: %.2f, ����Ʒ: %.2f, �ݳ�Ʒ: %.2f", outputView.m_FoodOutput, outputView.m_EveryDayNeedsOutput, outputView.m_LuxuryGoodsOutput);
			ImGui::Text(u8"ӯ��: ʳ��: %.2f, ����Ʒ: %.2f, �ݳ�Ʒ: %.2f", outputView.m_FoodOutput - outputView.m_FoodInput, 
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

	void PTMInspectorGUI::drawNation()
	{
		BEGIN_INSPECT(m_currInspectTownListNation, "Town List")
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
		END_INSPECT(m_currInspectNation)

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
				ImGui::PushID(hero);
				ImGui::Text(u8"%s ����:%s", hero->getName().c_str(), hero->getTownLocation()->getName().c_str());
				if(ImGui::Button("Set As Keeper"))
				{
					setInspectTownList(t, [t, hero](PTMTown* town){t->assignTownKeeper(town, hero);});
				}
				ImGui::SameLine();
				if(ImGui::Button("Assign On Duty"))
				{
					setInspectTownList(t, [t, hero](PTMTown* town){t->assignOnDuty(town, hero);});
				}
				ImGui::PopID();
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

}