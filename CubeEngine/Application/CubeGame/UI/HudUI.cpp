#include "HudUI.h"
#include "CubeGame/BuildingSystem.h"
#include "2D/GUISystem.h"
namespace tzw
{

	void HudUI::drawIMGUI(bool* isOpen)
	{
		vec2 screenSize = Engine::shared()->winSize();
		float yOffset = 15.0f;
		ImVec2 window_pos_pivot_bottom_right = ImVec2(1.0f, 1.0f);
		ImGui::SetNextWindowPos(ImVec2(screenSize.x - 50.0f, screenSize.y - yOffset), ImGuiCond_Always, window_pos_pivot_bottom_right);
		ImGui::Begin("Rotate Tips", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
		if (BuildingSystem::shared()->isIsInXRayMode())
		{
			ImGui::Text("XRay Mode");
		}
		else
		{
			ImGui::Text("Block Rotate");
		}
		ImGui::End();
	}

	HudUI::HudUI():m_onCreate(nullptr)
	{
	}

	void HudUI::prepare()
	{
	}

}
