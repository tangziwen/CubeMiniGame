#include "LoadWorldUI.h"
#include <filesystem>
namespace fs  = std::filesystem;
namespace tzw
{
	void LoadWorldUI::drawIMGUI(bool* isOpen)
	{
		auto screenSize = Engine::shared()->winSize();
		ImGui::SetNextWindowPos(ImVec2(screenSize.x / 2.0, screenSize.y / 2.0), ImGuiCond_Always, ImVec2(0.5, 0.5));
		ImGui::Begin(TRC(u8"∂¡»° ¿ΩÁ"),isOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
		ImGui::Text("WorldList:");
		for(const auto & entry : m_entryList)
		{
			auto worldName = entry.filename().string();
			if(ImGui::Button(worldName.c_str()))
			{
				if(m_onCreate)
				{
					m_onCreate(worldName);
				}
			}
		}
		ImGui::End();
	}
	LoadWorldUI::LoadWorldUI():m_onCreate(nullptr)
	{
	}

	void LoadWorldUI::prepare()
	{
		m_entryList.clear();
	    std::string path = "./Data/PlayerData/Save/";
		for (const auto& entry : fs::directory_iterator(path))
		{
			m_entryList.push_back(entry);
		}
	}

}