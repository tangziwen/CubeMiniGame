#include "GameUISystem.h"

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
#include "ThrusterPart.h"
#include "3D/Particle/ParticleEmitter.h"
#include "3D/Particle/ParticleUpdateColorModule.h"
#include "3D/Particle/ParticleInitAlphaModule.h"
#include "3D/Particle/ParticleInitLifeSpanModule.h"
#include "3D/Particle/ParticleUpdateSizeModule.h"
#include "3D/Particle/ParticleInitVelocityModule.h"
#include "3D/Particle/ParticleInitSizeModule.h"
#include "3D/Particle/ParticleInitPosModule.h"
#include "VehicleBroswer.h"
#include "PartSurfaceMgr.h"
#include "Base/TranslationMgr.h"
#include "2D/imgui_markdown.h"
#include "Utility/file/Tfile.h"
#include "LoadingUI.h"
#include "Action/ActionCalFunc.h"
#include "Base/TimerMgr.h"
#include "Engine/WorkerThreadSystem.h"
#include "ItemMgr.h"


namespace tzw {


static std::set<WindowType> m_currOpenWindowSet;
TZW_SINGLETON_IMPL(GameUISystem);
static void exitNow(Button * btn)
{
    exit(0);
}

static void onOption(Button * btn)
{

}
GameUISystem::GameUISystem(): m_isShowProfiler(false), m_isShowConsole(false),
	m_isOpenTerrain(false), m_isOpenRenderEditor(false),
	m_nodeEditor(nullptr), m_fileBrowser(nullptr),
	m_crossHair(nullptr),m_preIsNeedShow(false),m_isVisible(false),m_crossHairTipsInfo(nullptr)
	
{
	m_helperData = Tfile::shared()->getData("./Res/helpMain.md", true);
}

Texture * testIcon = nullptr;
void GameUISystem::init()
{
	EventMgr::shared()->addFixedPiorityListener(this);
	GUISystem::shared()->addObject(this);
	m_isShowProfiler = false;
	m_isShowConsole = false;
	m_nodeEditor = new GameNodeEditor();
	m_fileBrowser = new VehicleBroswer();
	m_fileBrowser->m_saveCallBack = [&](std::string fileName)
	{
		BuildingSystem::shared()->dump(fileName);
	};
	m_fileBrowser->m_loadCallBack = [&](std::string fileName)
	{
		m_nodeEditor->clearAll();
		BuildingSystem::shared()->load(fileName);
	};
	testIcon = TextureMgr::shared()->getByPath("./Texture/NodeEditor/ic_restore_white_24dp.png");
	//hide();
}

void GameUISystem::show()
{
	setVisible(true);
}

void GameUISystem::hide()
{
	if (GameWorld::shared()->getCurrentState() == GAME_STATE_RUNNING)
	{
		Engine::shared()->setUnlimitedCursor(true);
	}
	closeAllOpenedWindow();
}

void GameUISystem::toggle()
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

void GameUISystem::drawIMGUI()
{

	const float DISTANCE = 10.0f;
    static int corner = 2;
    ImGuiIO& io = ImGui::GetIO();
    if (corner != -1)
    {
        ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
        ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    }
    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    if (ImGui::Begin("OverLay", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
    {
        ImGui::Text(TRC(u8"Demo Version --tzw"));
    }
    ImGui::End();

	if(GameWorld::shared()->getCurrentState() == GAME_STATE_MAIN_MENU)
	{
		drawEntryInterFace();
	}

	auto currIsNeedShow = isVisible() || isNeedShowWindow();
	if(m_preIsNeedShow != currIsNeedShow)
	{
		if(GameWorld::shared()->getCurrentState() != GAME_STATE_MAIN_MENU)
		{
			if(currIsNeedShow)
			{
				Engine::shared()->setUnlimitedCursor(false);
				if(m_crossHair)
				{
					m_crossHair->setIsVisible(false);
				}
			}
			else
			{
				Engine::shared()->setUnlimitedCursor(true);
				if(m_crossHair)
				{
					m_crossHair->setIsVisible(true);
				}
			}
		}
		m_preIsNeedShow = currIsNeedShow;
	}
	if (isNeedShowWindow())
	{
		bool isOpenAbout = false;
		bool isOpenHelp = false;
		if(getWindowIsShow(WindowType::MainMenu))
		{
			if (ImGui::BeginMainMenuBarBottom())
			{
				if (ImGui::BeginMenu(u8"游戏"))
				{
					if (ImGui::MenuItem(u8"关闭菜单", "CTRL+Z")) {setWindowShow(WindowType::MainMenu, false);}
					if (ImGui::MenuItem(u8"清空所有", nullptr))
					{
						GameWorld::shared()->getPlayer()->removeAllBlocks();
						m_nodeEditor->clearAll();
					}
					if (ImGui::MenuItem(u8"退出游戏", "CTRL+Z")) { exit(0); }
					ImGui::EndMenu();
				}
				drawToolsMenu();
				static bool isOpenTerrain = false;
				if (ImGui::BeginMenu(u8"Debug"))
				{
					auto camera = g_GetCurrScene()->defaultCamera();
					ImGui::MenuItem(u8"性能剖析", nullptr, &m_isShowProfiler);
					ImGui::MenuItem(u8"控制台", nullptr, &m_isShowConsole);
					ImGui::MenuItem(u8"世界环境设置", nullptr, &m_isOpenTerrain);
					if (ImGui::MenuItem(u8"重载脚本", nullptr)) {ScriptPyMgr::shared()->reload();}
					if(ImGui::MenuItem("Particle test"))
					{
					auto node = Node::create();
					ParticleEmitter * emitter2 = new ParticleEmitter(40);
					emitter2->setSpawnRate(0.05);
					emitter2->addInitModule(new ParticleInitSizeModule(1.0, 1.0));
					emitter2->addInitModule(new ParticleInitVelocityModule(vec3(0, 1, 0), vec3(0, 1, 0)));
					emitter2->addInitModule(new ParticleInitLifeSpanModule(2.0, 2.0));
					emitter2->addInitModule(new ParticleInitAlphaModule(0.6, 0.6));
					emitter2->addUpdateModule(new ParticleUpdateSizeModule(1.0, 0.8));
					emitter2->addUpdateModule(new ParticleUpdateColorModule(vec4(0.36, 0.36, 0.5, 0.4), vec4(0.0, 0.0, 1.0, 0.01)));
					emitter2->setIsState(ParticleEmitter::State::Playing);
					node->addChild(emitter2);
					g_GetCurrScene()->addNode(node);
					node->setPos(camera->getPos() + camera->getForward() * 15.0f);
					node->setRotateE(vec3(0, 0, 45));
					}

					if(ImGui::MenuItem(u8"Reload Shader", nullptr, nullptr))
					{
						ShaderMgr::shared()->reloadAllShaders();
					}
					ImGui::MenuItem(u8"渲染设置", nullptr, &m_isOpenRenderEditor);
					ImGui::EndMenu();
				}
				if (m_isShowProfiler)
				{
					m_debugInfoPanel.drawIMGUI(&m_isShowProfiler);
				}
				if(m_isOpenRenderEditor)
				{
					ImGui::Begin("RenderEditor", &m_isOpenRenderEditor);
					if(ImGui::RadioButton("skyEnable", Renderer::shared()->isSkyEnable()))
					{
						Renderer::shared()->setSkyEnable(!Renderer::shared()->isSkyEnable());
					}
					if(ImGui::RadioButton("FogEnable", Renderer::shared()->isFogEnable()))
					{
						Renderer::shared()->setFogEnable(!Renderer::shared()->isFogEnable());
					}
					if(ImGui::RadioButton("SSAOEnable", Renderer::shared()->isSsaoEnable()))
					{
						Renderer::shared()->setSsaoEnable(!Renderer::shared()->isSsaoEnable());
					}
					if(ImGui::RadioButton("BloomEnable", Renderer::shared()->isBloomEnable()))
					{
						Renderer::shared()->setBloomEnable(!Renderer::shared()->isBloomEnable());
					}
					if(ImGui::RadioButton("HDREnable", Renderer::shared()->isHdrEnable()))
					{
						Renderer::shared()->setHdrEnable(!Renderer::shared()->isHdrEnable());
					}
					if(ImGui::RadioButton("AAEnable", Renderer::shared()->isAaEnable()))
					{
						Renderer::shared()->setAaEnable(!Renderer::shared()->isAaEnable());
					}
					if(ImGui::RadioButton("ShadowEnable", Renderer::shared()->isShadowEnable()))
					{
						Renderer::shared()->setShadowEnable(!Renderer::shared()->isShadowEnable());
					}
					ImGui::End();
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
			}
		}
		if (getWindowIsShow(WindowType::HELP_PAGE))
		{
			ImGui::MarkdownConfig config;
			bool isOpen = true;
			ImGui::Begin("Mark Down Test", &isOpen);
			ImGui::Markdown(m_helperData.getString().c_str(), m_helperData.getString().size(),config);
			ImGui::End();
			setWindowShow(WindowType::HELP_PAGE, isOpen);
		}
		if(getWindowIsShow(WindowType::NODE_EDITOR)) 
		{
			bool isOpen = true;
	        m_nodeEditor->drawIMGUI(&isOpen);
			setWindowShow(WindowType::NODE_EDITOR, isOpen);
		}

		if(getWindowIsShow(WindowType::INVENTORY))
		{
			bool isShow = ScriptPyMgr::shared()->callFunB("cpp_drawInventory");
			setWindowShow(WindowType::INVENTORY, isShow);
		}
		
		if(getWindowIsShow(WindowType::VEHICLE_FILE_BROWSER))
		{
			bool isOpen = true;
			m_fileBrowser->drawIMGUI(&isOpen);
			setWindowShow(WindowType::VEHICLE_FILE_BROWSER, isOpen);
        }

		if(getWindowIsShow(WindowType::RESUME_MENU))
		{
			auto screenSize = Engine::shared()->winSize();
			ImGui::SetNextWindowPos(ImVec2(screenSize.x / 2.0, screenSize.y / 2.0), ImGuiCond_Always, ImVec2(0.5, 0.5));
			ImGui::Begin(TRC(u8"是否继续?"),nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
			if(ImGui::Button(TRC(u8"继续"), ImVec2(160, 35)))
			{
				setWindowShow(WindowType::RESUME_MENU, false);
			}
			if(ImGui::Button(TRC(u8"保存世界"), ImVec2(160, 35)))
			{
				GameWorld::shared()->saveGame("World/testWord.json");
				setWindowShow(WindowType::RESUME_MENU, false);
			}
			//if(ImGui::Button(TRC(u8"读取世界"), ImVec2(160, 35)))
			//{
			//	GameWorld::shared()->loadGame("World/testWord.json");
			//	setWindowShow(WindowType::RESUME_MENU, false);
			//}
			if(ImGui::Button(TRC(u8"选项"), ImVec2(160, 35)))
			{
				m_option.open();
				setWindowShow(WindowType::OPTION_MENU, true);
				
				setWindowShow(WindowType::RESUME_MENU, false);
			}
			if(ImGui::Button(TRC(u8"退出"), ImVec2(160, 35)))
			{
				TranslationMgr::shared()->dump();
				exit(0);
			}
			ImGui::End();
		}

		//部件的属性面板
		if(getWindowIsShow(WindowType::ATTRIBUTE_WINDOW))
		{
			auto screenSize = Engine::shared()->winSize();
			ImGui::SetNextWindowPos(ImVec2(screenSize.x / 2.0, screenSize.y / 2.0), ImGuiCond_Always, ImVec2(0.5, 0.5));
			bool isOpen = true;
			ImGui::Begin(TRC(u8"属性面板"),&isOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
			m_curInspectPart->drawInspect();
			ImGui::End();
			setWindowShow(WindowType::ATTRIBUTE_WINDOW, isOpen);
		}

		//painter
		//喷漆部分
		if(getWindowIsShow(WindowType::PAINTER))
		{
			auto screenSize = Engine::shared()->winSize();
			ImGui::SetNextWindowPos(ImVec2(screenSize.x / 2.0, screenSize.y / 2.0), ImGuiCond_Always, ImVec2(0.5, 0.5));
			bool isOpen = true;
			ImGui::Begin(u8"Painter",&isOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
			auto col3 = GameWorld::shared()->getPlayer()->getPaintGun()->color;
			auto imCol4 = ImVec4(col3.x, col3.y, col3.z, 1.0f);
			ImGui::TextUnformatted(TRC(u8"颜色"));
			ImGui::ColorPicker4("MyColor##4", (float*)&imCol4, ImGuiColorEditFlags_NoAlpha,NULL);
			GameWorld::shared()->getPlayer()->getPaintGun()->color = vec3(imCol4.x, imCol4.y, imCol4.z);
			//表面材质
			ImGui::TextUnformatted(TRC(u8"表面材质"));
			auto size = PartSurfaceMgr::shared()->getItemAmount();
			for(int i = 0; i < size; i++)
			{
				auto surface = PartSurfaceMgr::shared()->getItemByIndex(i);
				auto p = GameWorld::shared()->getPlayer()->getPaintGun();
				if(ImGui::RadioButton(surface->getName().c_str(), surface ==p->m_surface))
				{
					p->m_surface = surface;
				}
			}
			if(ImGui::Button("Make As Template"))
			{
				//pass
				auto item = GameWorld::shared()->getPlayer()->getCurSelectedItem();
				GameItem * newItem = new GameItem(*item);
				newItem->m_name = item->m_name + " New";
				ItemMgr::shared()->pushItem(newItem);
			}
			ImGui::End();
			setWindowShow(WindowType::PAINTER, isOpen);
		}


		//选项面板
		if(getWindowIsShow(WindowType::OPTION_MENU)) 
		{
			auto screenSize = Engine::shared()->winSize();
			ImGui::SetNextWindowPos(ImVec2(screenSize.x / 2.0, screenSize.y / 2.0), ImGuiCond_Always, ImVec2(0.5, 0.5));
			bool isOpen = true;
			m_option.drawIMGUI(&isOpen);
			setWindowShow(WindowType::OPTION_MENU, isOpen);
		}

		//quick Debug
		if(getWindowIsShow(WindowType::QUICK_DEBUG)) 
		{
			auto screenSize = Engine::shared()->winSize();
			ImGui::SetNextWindowPos(ImVec2(screenSize.x / 2.0, screenSize.y / 2.0), ImGuiCond_Always, ImVec2(0.5, 0.5));
			bool isOpen = true;
			ImGui::Begin(u8"Painter",&isOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);

			auto ambient = g_GetCurrScene()->getAmbient();
			float intensity = ambient->intensity();
			if(ImGui::DragFloat("IBL", &intensity, 0.01, 0.0f, 5.0f)) 
			{
				ambient->setIntensity(intensity);
			}
			ImGui::End();
			setWindowShow(WindowType::QUICK_DEBUG, isOpen);
		}
		
	}

}

bool GameUISystem::onKeyPress(int keyCode)
{
	return true;
}

bool GameUISystem::isVisible() const
{
	return m_isVisible;
}

void GameUISystem::setVisible(bool val)
{
	m_isVisible = val;
}

GameNodeEditor* GameUISystem::getNodeEditor()
{
	return m_nodeEditor;
}

void GameUISystem::setIsShowNodeEditor(bool isShow)
{
	setWindowShow(WindowType::NODE_EDITOR, isShow);

}

void GameUISystem::setIsShowAssetEditor(bool isShow)
{
	//m_isOpenAssetEditor = true;
	//m_currOpenWindowSet.insert(WindowType::INVENTORY);
	setWindowShow(WindowType::INVENTORY, isShow);
}

void GameUISystem::popFloatTips(std::string floatString)
{
	UIHelper::shared()->showFloatTips(floatString);
}

void GameUISystem::closeAllOpenedWindow()
{
	m_isOpenTerrain = false;
	m_currOpenWindowSet.clear();
}

bool GameUISystem::isNeedShowWindow()
{
	return m_currOpenWindowSet.size() > 0;
}

bool GameUISystem::isAnyShow()
{
	return isVisible() || isNeedShowWindow();
}

void GameUISystem::setIsFileBroswerOpen(bool isOpen)
{
	setWindowShow(WindowType::VEHICLE_FILE_BROWSER, isOpen);
}

void GameUISystem::startGame()
{
	hide();
    GameWorld::shared()->startGame();
}

void GameUISystem::drawToolsMenu()
{
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

void GameUISystem::ShowExampleAppLog(bool* p_open)
{
	static ExampleAppLog log;
	log.Draw("LogMenu", p_open);
}

void GameUISystem::ShowExampleAppConsole(bool* p_open)
{
	ConsolePanel::shared()->Draw("Console", p_open);
}

void GameUISystem::drawInventory()
{
	
}

bool GameUISystem::isOpenAssetEditor() const
{
	return getWindowIsShow(WindowType::INVENTORY);
}

void GameUISystem::closeCurrentWindow()
{
	if(m_currOpenWindowSet.size() > 0)
	{
		m_currOpenWindowSet.erase(m_currOpenWindowSet.begin());
	}
	else
	{
		//
		setWindowShow(WindowType::RESUME_MENU, true);	
	}
}

void GameUISystem::setWindowShow(WindowType type, bool isShow)
{
	if(isShow)
	{
		m_currOpenWindowSet.insert(type);
	}else
	{
		if(m_currOpenWindowSet.find(type) != m_currOpenWindowSet.end())
		{
			m_currOpenWindowSet.erase(m_currOpenWindowSet.find(type));
		}
	}
}

bool GameUISystem::getWindowIsShow(WindowType type) const
{
	return m_currOpenWindowSet.find(type) != m_currOpenWindowSet.end();
}

void GameUISystem::openInspectWindow(GamePart* part)
{
	setWindowShow(WindowType::ATTRIBUTE_WINDOW, true);
	m_curInspectPart = part;
}

void GameUISystem::setPainterShow(bool isShow)
{
	setWindowShow(WindowType::PAINTER, isShow);
}

void GameUISystem::drawEntryInterFace()
{
	auto screenSize = Engine::shared()->winSize();
	ImGui::SetNextWindowPos(ImVec2(screenSize.x / 2.0, screenSize.y / 2.0), ImGuiCond_Always, ImVec2(0.5, 0.5));
	if (ImGui::Begin("CubeEngine",0, ImGuiWindowFlags_NoMove| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse))
	{

		if(ImGui::Button(TRC(u8"创建新世界"), ImVec2(160, 35)))
		{
			auto fuck = []
			{
				GameWorld::shared()->startGame();
				WorkerThreadSystem::shared()->pushMainThreadOrder(WorkerJob([&](){LoadingUI::shared()->hide();}));
			};
			TimerMgr::shared()->addTimer(new Timer(fuck, 0.1f));
			//runa
			LoadingUI::shared()->show();
			
			Engine::shared()->setUnlimitedCursor(true);
		}
		ImGui::Spacing();
		if(ImGui::Button(TRC(u8"载入世界"), ImVec2(160, 35)))
		{
			auto fuck = []
			{
				GameWorld::shared()->loadGame("World/testWord.json");
				WorkerThreadSystem::shared()->pushMainThreadOrder(WorkerJob([&](){LoadingUI::shared()->hide();}));
			};
			TimerMgr::shared()->addTimer(new Timer(fuck, 0.1f));
			//runa
			LoadingUI::shared()->show();
			Engine::shared()->setUnlimitedCursor(true);
		}
		ImGui::Spacing();
		if(ImGui::Button(TRC(u8"选项"), ImVec2(160, 35)))
		{
			m_option.open();
			setWindowShow(WindowType::OPTION_MENU, true);
		}
		ImGui::Spacing();
		if(ImGui::Button(TRC(u8"帮助"), ImVec2(160, 35)))
		{
			auto a = new Texture();
                  a->loadAsync(
                    "Texture/modern-tile1-ue/modern-tile1-albedo.png", 
					  [a](Texture * tex)
					{
						tlog("yes we finished");
						auto sprite = new Sprite();
						sprite->initWithTexture(tex);
						sprite->setContentSize(vec2(200, 200));
						g_GetCurrScene()->addNode(sprite);
					}
				  );
		}
		ImGui::Spacing();
		if(ImGui::Button(TRC(u8"退出"), ImVec2(160, 35))) 
		{
			TranslationMgr::shared()->dump();
			exit(0);
		}
		ImGui::Spacing();
		ImGui::End();
	}
}

LabelNew* GameUISystem::getCrossHairTipsInfo() const
{
	return m_crossHairTipsInfo;
}

void GameUISystem::initInGame()
{
    m_crossHair = Sprite::create("Texture/cross_hair.png");
	 
    auto size = m_crossHair->getContentSize();
	
    m_crossHair->setPos2D(Engine::shared()->windowWidth()/2 - size.x/2,Engine::shared()->windowHeight()/2 - size.y/2);
	 
    GameWorld::shared()->getMainRoot()->addChild(m_crossHair);
	m_crossHairTipsInfo = LabelNew::create(TRC(u8"暂无提示"));
	GameWorld::shared()->getMainRoot()->addChild(m_crossHairTipsInfo);
	m_crossHairTipsInfo->setIsVisible(false);
	m_crossHairTipsInfo->setPos2D(Engine::shared()->windowWidth()/2 - size.x/2,Engine::shared()->windowHeight()/2 - size.y/2 -35);
	// m_crossHair->addChild(m_crossHairTipsInfo);
}
} // namespace tzw
