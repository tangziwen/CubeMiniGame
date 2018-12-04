#include "MainMenu.h"

#include "EngineSrc/CubeEngine.h"
#include "CubeGame/GameWorld.h"
#include "Game/ConsolePanel.h"
#include "Event/EventMgr.h"
#include "Technique/MaterialPool.h"
#include "3D/Sky.h"
#include "Collision/PhysicsMgr.h"
#include "Utility/math/TbaseMath.h"
#include "BuildingSystem.h"
#include "2D/GUISystem.h"
namespace tzw {
TZW_SINGLETON_IMPL(MainMenu);
static void exitNow(Button * btn)
{
    exit(0);
}

static void onOption(Button * btn)
{

}

MainMenu::MainMenu(): m_isShowProfiler(false), m_isShowConsole(false)
{
}

void MainMenu::init()
{
	EventMgr::shared()->addFixedPiorityListener(this);
	GUISystem::shared()->addObject(this);
	m_isShowProfiler = false;
	m_isShowConsole = false;
	//hide();
}

void MainMenu::show()
{
	Engine::shared()->setUnlimitedCursor(false);
	setVisible(true);
}

void MainMenu::hide()
{
	if (GameWorld::shared()->getCurrentState() == GameWorld::GameState::OnPlay)
	{
		Engine::shared()->setUnlimitedCursor(true);
	}
	setVisible(false);
    //setIsVisible(false);
}

void MainMenu::toggle()
{

	if (isVisible())
	{
		hide();
	}
	else
	{
		show();
	}
}

void MainMenu::drawIMGUI()
{
	if (isVisible())
	{
		bool isOpenAbout = false;
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Game"))
			{
				if (ImGui::MenuItem("Start", "CTRL+Z")) { startGame(); }
				if (ImGui::MenuItem("Load", "CTRL+Z")) {}
				if (ImGui::MenuItem("Option", "CTRL+Z")) {}
				if (ImGui::MenuItem("Exit", "CTRL+Z")) { exit(0); }
				ImGui::EndMenu();
			}
			drawToolsMenu();
			static bool isOpenTerrain = false;
			if (ImGui::BeginMenu("Run-time Config"))
			{

				ImGui::MenuItem("terrain", nullptr, &isOpenTerrain);
				ImGui::EndMenu();
			}
			if (isOpenTerrain)
			{
				ImGui::Begin("Terrain Inspector", &isOpenTerrain);
				ImGui::Text("Terrain Inspector");
				auto terrainMat = MaterialPool::shared()->getMatFromTemplate("VoxelTerrain");
				terrainMat->inspect();

				auto fogMat = MaterialPool::shared()->getMaterialByName("GlobalFog");
				fogMat->inspect();

				auto PostMat = MaterialPool::shared()->getMaterialByName("SSAO");
				PostMat->inspect();

				auto sunMat = Sky::shared()->getMaterial();
				sunMat->inspectIMGUI("sun_intensity", 0.0f, 100.0f);
				auto dirLight = g_GetCurrScene()->getDirectionLight();
				float sunAngle2 = TbaseMath::Radius2Ang(dirLight->phi());
				ImGui::SliderFloat("Sun Angle", &sunAngle2, -180, 180);
				dirLight->setPhi(TbaseMath::Ang2Radius(sunAngle2));

				if (ImGui::Button("dump to file"))
				{
					//
				}
				ImGui::End();
			}
			if (ImGui::BeginMenu("Test"))
			{

				if (ImGui::MenuItem("test phys", nullptr))
				{
					PhysicsMgr::shared()->start();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("?"))
			{
				if (ImGui::MenuItem("About", nullptr)) {
					isOpenAbout = true;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();

			if (isOpenAbout) ImGui::OpenPopup("About");
			if (ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Cube-Engine By Tzw.\ntzwtangziwen@163.com\nhttps://github.com/tangziwen/Cube-Engine");
				ImGui::Separator();
				if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}
		}
	}
}

bool MainMenu::onKeyPress(int keyCode)
{
	if (keyCode == TZW_KEY_GRAVE_ACCENT)
		toggle();
	return true;
}

bool MainMenu::isVisible() const
{
	return m_isVisible;
}

void MainMenu::setVisible(bool val)
{
	m_isVisible = val;
}

void MainMenu::startGame()
{
    GameWorld::shared()->startGame();
	hide();
}

void MainMenu::drawToolsMenu()
{
	if (ImGui::BeginMenu("Tools"))
	{
		ImGui::MenuItem("Profiler", nullptr, &m_isShowProfiler);
		ImGui::MenuItem("Console", nullptr, &m_isShowConsole);
		ImGui::EndMenu();
	}
	if (m_isShowProfiler)
	{
		m_debugInfoPanel.drawIMGUI(&m_isShowProfiler);
	}
	if (m_isShowConsole)
	{
		ShowExampleAppConsole(&m_isShowConsole);
	}
}


struct ExampleAppLog
{
	ImGuiTextBuffer     Buf;
	ImGuiTextFilter     Filter;
	ImVector<int>       LineOffsets;        // Index to lines offset
	bool                ScrollToBottom;

	void    Clear() { Buf.clear(); LineOffsets.clear(); }

	void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
	{
		int old_size = Buf.size();
		va_list args;
		va_start(args, fmt);
		Buf.appendfv(fmt, args);
		va_end(args);
		for (int new_size = Buf.size(); old_size < new_size; old_size++)
			if (Buf[old_size] == '\n')
				LineOffsets.push_back(old_size);
		ScrollToBottom = true;
	}

	void    Draw(const char* title, bool* p_open = nullptr)
	{
		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
		ImGui::Begin(title, p_open);
		if (ImGui::Button("Clear")) Clear();
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::SameLine();
		Filter.Draw("Filter", -100.0f);
		ImGui::Separator();
		ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
		if (copy) ImGui::LogToClipboard();

		if (Filter.IsActive())
		{
			const char* buf_begin = Buf.begin();
			const char* line = buf_begin;
			for (int line_no = 0; line != nullptr; line_no++)
			{
				const char* line_end = (line_no < LineOffsets.Size) ? buf_begin + LineOffsets[line_no] : nullptr;
				if (Filter.PassFilter(line, line_end))
					ImGui::TextUnformatted(line, line_end);
				line = line_end && line_end[1] ? line_end + 1 : nullptr;
			}
		}
		else
		{
			ImGui::TextUnformatted(Buf.begin());
		}

		if (ScrollToBottom)
			ImGui::SetScrollHere(1.0f);
		ScrollToBottom = false;
		ImGui::EndChild();
		ImGui::End();
	}
};

void MainMenu::ShowExampleAppLog(bool* p_open)
{
	static ExampleAppLog log;

	// Demo: add random items (unless Ctrl is held)
	//static float last_time = -1.0f;
	//float time = ImGui::GetTime();
	//if (time - last_time >= 0.20f && !ImGui::GetIO().KeyCtrl)
	//{
	//	const char* random_words[] = { "system", "info", "warning", "error", "fatal", "notice", "log" };
	//	log.AddLog("[%s] Hello, time is %.1f, frame count is %d\n", random_words[rand() % IM_ARRAYSIZE(random_words)], time, ImGui::GetFrameCount());
	//	last_time = time;
	//}
	log.Draw("LogMenu", p_open);
}


void MainMenu::ShowExampleAppConsole(bool* p_open)
{
	ConsolePanel::shared()->Draw("Console", p_open);
}
} // namespace tzw
