#include "LoadWorldUI.h"

namespace tzw
{
	void LoadWorldUI::drawIMGUI(bool* isOpen)
	{
		auto screenSize = Engine::shared()->winSize();
		ImGui::SetNextWindowPos(ImVec2(screenSize.x / 2.0, screenSize.y / 2.0), ImGuiCond_Always, ImVec2(0.5, 0.5));
		ImGui::Begin(TRC(u8"∂¡»° ¿ΩÁ"),isOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
		if(ImGui::InputText("World Name",m_worldName, 64))
		{
		}
		if(ImGui::Button("Load"))
		{
			if(m_onCreate)
			{
				m_onCreate(m_worldName);
			}
		}
		ImGui::End();
	}
	LoadWorldUI::LoadWorldUI():m_onCreate(nullptr)
	{
	}

}