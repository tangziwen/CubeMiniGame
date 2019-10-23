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
#include "../../EngineSrc/2D/imnodes.h"
#include "ScriptPy/ScriptPyMgr.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"

#include "UIHelper.h"

#include "dirent.h"
#include "Shader/ShaderMgr.h"
#include "2D/imgui_internal.h"
#include "algorithm"
namespace tzw {
TZW_SINGLETON_IMPL(MainMenu);
static void exitNow(Button * btn)
{
    exit(0);
}

static void onOption(Button * btn)
{

}

MainMenu::MainMenu(): m_isShowProfiler(false), m_isShowConsole(false), m_isShowNodeEditor(false),
					m_isOpenTerrain(false), m_isOpenAssetEditor(false), m_nodeEditor(nullptr), m_fileBrowser(nullptr),
					m_crossHair(nullptr)
{
}
Texture * testIcon = nullptr;
void MainMenu::init()
{
	EventMgr::shared()->addFixedPiorityListener(this);
	GUISystem::shared()->addObject(this);
	m_isShowProfiler = false;
	m_isShowConsole = false;
	m_nodeEditor = new GameNodeEditor();
	m_fileBrowser = new GUIFileBrowser();
	testIcon = TextureMgr::shared()->getByPath("./Texture/NodeEditor/ic_restore_white_24dp.png");
	//hide();
}

void MainMenu::show()
{
	Engine::shared()->setUnlimitedCursor(false);
	setVisible(true);
	if(m_crossHair)
	{
		m_crossHair->setIsVisible(false);
	}
	
}

void MainMenu::hide()
{
	if (GameWorld::shared()->getCurrentState() == GAME_STATE_RUNNING)
	{
		Engine::shared()->setUnlimitedCursor(true);
	}
	setVisible(false);
	if(m_crossHair)
	{
		m_crossHair->setIsVisible(true);	
	}
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
		bool isOpenHelp = false;
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu(u8"游戏"))
			{
				if (ImGui::MenuItem(u8"开始", "CTRL+Z")) { startGame(); }
				if (ImGui::MenuItem(u8"保存载具", "CTRL+Z"))
				{
					m_fileBrowser->open(u8"保存为", u8"保存");
					m_fileBrowser->m_callBack = [&](std::string fileName)
					{
						BuildingSystem::shared()->dump(fileName);
						m_fileBrowser->close();
					};
					
				}
				if (ImGui::MenuItem(u8"读取载具", "CTRL+Z"))
				{
					m_fileBrowser->open(u8"选择载具文件", u8"打开");
					m_fileBrowser->m_callBack = [&](std::string fileName)
					{
						BuildingSystem::shared()->load(fileName);
						m_fileBrowser->close();
					};
					//BuildingSystem::shared()->load();
				}
				if (ImGui::MenuItem(u8"选项", "CTRL+Z")) {}
				if (ImGui::MenuItem(u8"重载脚本", "CTRL+Z")) {ScriptPyMgr::shared()->reload();}
				if (ImGui::MenuItem(u8"退出", "CTRL+Z")) { exit(0); }
				ImGui::EndMenu();
			}
			drawToolsMenu();
			static bool isOpenTerrain = false;
			if (ImGui::BeginMenu(u8"调试工具"))
			{
				ImGui::MenuItem(u8"性能剖析", nullptr, &m_isShowProfiler);
				ImGui::MenuItem(u8"控制台", nullptr, &m_isShowConsole);


				if(ImGui::MenuItem(u8"Reload Shader", nullptr, nullptr))
				{
					ShaderMgr::shared()->reloadAllShaders();
				}
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

			if (ImGui::BeginMenu("?"))
			{
				if (ImGui::MenuItem(u8"关于", nullptr)) {
					isOpenAbout = true;
				}
				if (ImGui::MenuItem(u8"帮助", nullptr)) {
					isOpenHelp = true;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();

			if (isOpenAbout) 
			{
                ImGui::OpenPopup(u8"关于");
				isOpenAbout = false;
            }
			if (ImGui::BeginPopupModal(u8"关于", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Cube-Engine By Tzw.\ntzwtangziwen@163.com\nhttps://github.com/tangziwen/Cube-Engine");
				ImGui::Separator();
				if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}


			if (isOpenHelp)
			{

				ScriptPyMgr::shared()->callFunV("showHelpPage");
			}
		}

		if(m_isShowNodeEditor) 
		{
	        m_nodeEditor->drawIMGUI(&m_isShowNodeEditor);
		}
		if(m_isOpenAssetEditor)
		{
			ImGui::Begin(u8"资产浏览器");
			for(int i =0; i < 25; i++)
			{
				//
				auto s = ImGui::GetStyle();
				auto spaceX = s.ItemSpacing.x;
				auto padding = s.FramePadding.x;
				auto size = ImGui::GetWindowWidth() - s.IndentSpacing;
				int coloum = int(size / (50 + spaceX * 2 + padding * 2)) + 1;
				if ((i % coloum) != 0)
					ImGui::SameLine();
				ImGui::PushID(i);
				ImGui::BeginGroup();
				ImGui::ImageButton(reinterpret_cast<ImTextureID> (testIcon->handle()), ImVec2(50, 50));
				//ImGui::Button("ABCDEFG", ImVec2(50, 50));
                // Our buttons are both drag sources and drag targets here!
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                {
                    ImGui::SetDragDropPayload("DND_DEMO_CELL", &i, sizeof(int));    // Set payload to carry the index of our item (could be anything)
					ImGui::Text("Copy %d", i);   // Display preview (could be anything, e.g. when dragging an image we could decide to display the filename and a small preview of the image, etc.)
                    ImGui::EndDragDropSource();
                }
				ImGui::Text("fuck %d", i);
				ImGui::EndGroup();
				ImGui::PopID();
                if (ImGui::BeginDragDropTarget())
                {
                	tlog("aaaaaaaaaaa");
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL")) 
					{
                    	tlog("drop!!!!!");
					}
					ImGui::EndDragDropTarget();
                }
			}
			ImGui::End();
		}
		m_fileBrowser->drawIMGUI();
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

GameNodeEditor* MainMenu::getNodeEditor()
{
	return m_nodeEditor;
}

void MainMenu::setIsShowNodeEditor(bool isShow)
{
	m_isShowNodeEditor = isShow;
}

void MainMenu::popFloatTips(std::string floatString)
{
	UIHelper::shared()->showFloatTips(floatString);
}

void MainMenu::startGame()
{
    GameWorld::shared()->startGame();
	hide();
}

void MainMenu::drawToolsMenu()
{
	if (ImGui::BeginMenu(u8"工具"))
	{
		ImGui::MenuItem(u8"节点编辑器", nullptr, &m_isShowNodeEditor);
		ImGui::MenuItem(u8"资产浏览器", nullptr, &m_isOpenAssetEditor);
		ImGui::MenuItem(u8"世界环境设置", nullptr, &m_isOpenTerrain);
		ImGui::EndMenu();
	}
	if (m_isOpenTerrain)
	{
		ImGui::Begin("Terrain Inspector", &m_isOpenTerrain);
		ImGui::Text("Terrain Inspector");
		auto bloomComposisit = MaterialPool::shared()->getMatFromTemplate("BloomCompositePass");
		bloomComposisit->inspect();
		
		auto terrainMat = MaterialPool::shared()->getMatFromTemplate("VoxelTerrain");
		terrainMat->inspect();

		auto ambient = g_GetCurrScene()->getAmbient();
		auto ambientIntensity = ambient->intensity();

		ImGui::SliderFloat("Ambient Intensity", &ambientIntensity, 0.0, 50.0);
		ambient->setIntensity(ambientIntensity);
		auto fogMat = MaterialPool::shared()->getMaterialByName("GlobalFog");
		fogMat->inspect();

		auto PostMat = MaterialPool::shared()->getMaterialByName("SSAO");
		PostMat->inspect();

		//auto sunMat = Sky::shared()->getMaterial();
		//sunMat->inspectIMGUI("sun_intensity", 0.0f, 100.0f);
		auto dirLight = g_GetCurrScene()->getDirectionLight();
		float sunAngle2 = TbaseMath::Radius2Ang(dirLight->phi());
		ImGui::SliderFloat("Sun Angle", &sunAngle2, -180, 180);
		dirLight->setPhi(TbaseMath::Ang2Radius(sunAngle2));

		float sunIntensity = dirLight->intensity();
		ImGui::SliderFloat("SunIntensity", &sunIntensity, 0.0, 50.0);
		dirLight->setIntensity(sunIntensity);
		ImGui::End();
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
	log.Draw("LogMenu", p_open);
}

void MainMenu::ShowExampleAppConsole(bool* p_open)
{
	ConsolePanel::shared()->Draw("Console", p_open);
}

void MainMenu::initInGame()
{
    m_crossHair = Sprite::create("Texture/cross_hair.png");
	 
    auto size = m_crossHair->getContentSize();
	 
    m_crossHair->setPos2D(Engine::shared()->windowWidth()/2 - size.x/2,Engine::shared()->windowHeight()/2 - size.y/2);
	 
    GameWorld::shared()->getMainRoot()->addChild(m_crossHair);
}
} // namespace tzw
