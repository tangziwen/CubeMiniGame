#include "NewWorldSettingUI.h"

namespace tzw
{
	void NewWorldSettingUI::drawIMGUI(bool* isOpen)
	{
		auto screenSize = Engine::shared()->winSize();
		ImGui::SetNextWindowPos(ImVec2(screenSize.x / 2.0, screenSize.y / 2.0), ImGuiCond_Always, ImVec2(0.5, 0.5));
		ImGui::Begin(TRC(u8"创建世界设定"),isOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
		if(ImGui::InputText("World Name",m_worldInfo.m_gameName, 64))
		{
		}
		ImGui::Text("Procedural Terrain Type:");
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC("Flat World"), m_worldInfo.m_terrainProceduralType == 0))
		{
			m_worldInfo.m_terrainProceduralType = 0;
		}
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC("Wild Land World"), m_worldInfo.m_terrainProceduralType == 1))
		{
			m_worldInfo.m_terrainProceduralType = 1;
		}

		ImGui::Text("GamePlay Type:");
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC("Survival Mode"), m_worldInfo.m_gameMode == 0))
		{
			m_worldInfo.m_gameMode = 0;
		}
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC("SandBox Mode"), m_worldInfo.m_gameMode == 1))
		{
			m_worldInfo.m_gameMode = 1;
		}

		if(ImGui::Button("Create"))
		{
			if(m_onCreate)
			{
				m_onCreate(m_worldInfo);
			}
		}
		ImGui::End();
	}
	NewWorldSettingUI::NewWorldSettingUI():m_onCreate(nullptr)
	{
	}
	WorldInfo NewWorldSettingUI::getWorldInfo() const
	{
		return m_worldInfo;
	}

}