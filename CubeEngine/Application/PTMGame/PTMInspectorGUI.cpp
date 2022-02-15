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

	static bool DrawButtonWithTips(const char * btnstr, const char * tips)
	{
		bool isClicked = ImGui::Button(btnstr);
		if(ImGui::IsItemHovered()&& GImGui->HoveredIdTimer > 0.3)
		{
			ImGui::SetTooltip(tips);
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
		BEGIN_INSPECT(m_currInspectNation, "Nation")
			PTMNation * t = m_currInspectNation;
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

		END_INSPECT(m_currInspectNation)

		BEGIN_INSPECT(m_currInspectTown, "Town")
			PTMTown * t = m_currInspectTown;
			ImGui::Text("Name: %s ", t->getName().c_str());
				ImGui::SameLine();
				DrawNationTitle(t->getOwner());

			ImGui::BeginGroupPanel("Economy");
			DRAW_PROPERTY(t, EcoDevLevel)
				ImGui::SameLine();
				ImGui::Button("Boost Eco");
			DRAW_PROPERTY(t, Autonomous)
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel("Military");
				DRAW_PROPERTY(t, MilDevLevel)
					ImGui::SameLine();
					bool isControlledByPlayer = PTMWorld::shared()->getPlayerController()->getControlledNation() == t->getOwner();
					if(!isControlledByPlayer)
					{
						ImGuiContext& g = *GImGui;
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::PushStyleVar(ImGuiStyleVar_Alpha, g.Style.Alpha * 0.6f);
					}
					ImGui::Button("Boost Mil");

					DrawButtonWithTips("Build Army", "Build An Army");
					DrawButtonWithTips("Suprress Unrest", "Build An Army");
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
			DrawButtonWithTips("Issue Edict", "Build An Army");
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

}