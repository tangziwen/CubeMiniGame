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
#include "UI/KeyMapper.h"
#include "UI/LoadWorldUI.h"
#include "UI/NewWorldSettingUI.h"
#include "UI/PainterUI.h"


namespace tzw {


static std::set<WindowType> m_currOpenWindowSet;

static void exitNow(Button * btn)
{
    exit(0);
}

static void onOption(Button * btn)
{

}
GameUISystem::GameUISystem(): m_isShowProfiler(false), m_isShowConsole(false),
	m_isOpenTerrain(false), m_isOpenRenderEditor(false),
	m_fileBrowser(nullptr),
	m_crossHair(nullptr),m_preIsNeedShow(false),m_isVisible(false),m_crossHairTipsInfo(nullptr),m_isOpenPlayerOverLay(true)
	
{
	m_helperData = Tfile::shared()->getData("helpMain.md", true);
}

	Texture * testIcon = nullptr;
	Texture * settingIcon = nullptr;
	Texture * bluePrintIcon = nullptr;
	Texture * helpIcon = nullptr;
	Texture * homeIcon = nullptr;
	Texture * packageIcon = nullptr;
void GameUISystem::init()
{
	EventMgr::shared()->addFixedPiorityListener(this);
	GUISystem::shared()->addObject(this);
	m_isShowProfiler = false;
	m_isShowConsole = false;
	m_fileBrowser = new VehicleBroswer();
	m_NewWorldSettingUI = new NewWorldSettingUI();
	
	m_NewWorldSettingUI->m_onCreate = [this](WorldInfo info)
	{
		auto fuck = [info, this ]
		{
			closeAllOpenedWindow();
			GameWorld::shared()->startGame(info);
			WorkerThreadSystem::shared()->pushMainThreadOrder(WorkerJob([&](){LoadingUI::shared()->hide();}));
		};
		TimerMgr::shared()->addTimer(new Timer(fuck, 0.1f));
		//runa
		LoadingUI::shared()->show();
		
		Engine::shared()->setUnlimitedCursor(true);
	};
	m_loadWorldUI = new LoadWorldUI();
	m_loadWorldUI->m_onCreate = [this](std::string worldName)
	{
		auto fuck = [worldName, this ]
		{
			closeAllOpenedWindow();
			GameWorld::shared()->loadGame(worldName);
			WorkerThreadSystem::shared()->pushMainThreadOrder(WorkerJob([&](){LoadingUI::shared()->hide();}));
		};
		TimerMgr::shared()->addTimer(new Timer(fuck, 0.1f));
		//runa
		LoadingUI::shared()->show();
		Engine::shared()->setUnlimitedCursor(true);
	};
	m_painterUI = new PainterUI();
	m_fileBrowser->m_saveCallBack = [&](std::string fileName)
	{
		BuildingSystem::shared()->dumpVehicle(fileName);
	};
	m_fileBrowser->m_loadCallBack = [&](std::string fileName)
	{
		BuildingSystem::shared()->loadVehicle(fileName);
	};
	testIcon = TextureMgr::shared()->getByPath("UITexture/NodeEditor/ic_restore_white_24dp.png");
	settingIcon = TextureMgr::shared()->getByPath("UITexture/Icon/icons8-gear-96.png");
	bluePrintIcon = TextureMgr::shared()->getByPath("UITexture/Icon/icons8-blueprint-96.png");
	helpIcon = TextureMgr::shared()->getByPath("UITexture/Icon/icons8-help-96.png");
	homeIcon = TextureMgr::shared()->getByPath("UITexture/Icon/icons8-home-page-96.png");
	packageIcon = TextureMgr::shared()->getByPath("UITexture/Icon/icons8-furniture-store-96.png");
	
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

	if(GameWorld::shared()->getCurrentState() == GAME_STATE_MAIN_MENU)
	{
		drawEntryInterFace();
	}else
	{
	//KeyMapper
		{
			if(KeyMapper::shared()->isOpen())
			{
				KeyMapper::shared()->drawIMGUI(nullptr);
			}
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
	else
	{

	}
	}


	if (isNeedShowWindow())
	{
		bool isOpenAbout = false;
		bool isOpenHelp = false;
		if(getWindowIsShow(WindowType::MainMenu))
		{
			auto io = ImGui::GetIO();
			auto style = ImGui::GetStyle();
			ImGui::SetNextWindowPos(ImVec2(0.0f, 0));
			ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, 0.0f));
			ImGui::SetNextWindowBgAlpha(0.75f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings; //| ImGuiWindowFlags_MenuBar;
			ImGui::Begin("MenuBar",nullptr, window_flags);
			ImGui::PopStyleVar(2);
			GUISystem::shared()->imguiUseSmallFont();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			float IconSize = 32.0f;
			ImVec2 homeDropDownPos;
			ImGui::BeginHorizontal("Menu");
			//ImGui::BeginHorizontal("IconGroup");
			if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(homeIcon->getTextureId()), ImVec2(IconSize,IconSize)))
			//if(ImGui::SmallButton("Menu"))
			{
				//closeAllOpenedWindow();
				setWindowShow(WindowType::RESUME_MENU, true);
				//ImGui::OpenPopup("HomePopup");
			}
			//ImGui::SameLine();
			homeDropDownPos = ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y);
			//ImGui::Spacing();
			//ImGui::SameLine();
			
			//if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(packageIcon->getTextureId()), ImVec2(IconSize,IconSize), ImVec2(0,1), ImVec2(1,0)))
			
			if(ImGui::Button("Inventory",ImVec2(0, IconSize)))
			{
				setWindowShow(WindowType::INVENTORY, true);
			}
			homeDropDownPos = ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y);
			//ImGui::SameLine();
			if(ImGui::BeginMenu("Misc", true, ImGui::GetItemRectSize().y))
			{
				if (ImGui::MenuItem(TRC("WorldSetting"),NULL,getWindowIsShow(WindowType::WORLD_SETTING))) 
				{
					setWindowShow(WindowType::WORLD_SETTING, true);
				}
                if (ImGui::MenuItem("Console",0,getWindowIsShow(WindowType::Console))) 
				{
                	setWindowShow(WindowType::Console, true);
                }
				ImGui::EndMenu();
			}
			//now with right
			ImGui::SetCursorPosX(io.DisplaySize.x - 100.0f);
			if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(settingIcon->getTextureId()), ImVec2(IconSize,IconSize)))
			{
				setWindowShow(WindowType::OPTION_MENU, true);
			}
			//ImGui::SameLine();
			if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(helpIcon->getTextureId()), ImVec2(IconSize,IconSize)))
			{
				setWindowShow(WindowType::HELP_PAGE, true);
			}
			ImGui::EndHorizontal();
			ImGui::SetNextWindowPos(homeDropDownPos);
			if(ImGui::BeginPopup("HomePopup"))
			{
				ImGui::PushID("LoadGame");
				ImGui::ImageButton(reinterpret_cast<ImTextureID>(packageIcon->getTextureId()), ImVec2(IconSize,IconSize));
				ImGui::PopID();
				ImGui::PushID("SaveGame");
				ImGui::ImageButton(reinterpret_cast<ImTextureID>(packageIcon->getTextureId()), ImVec2(IconSize,IconSize));
				ImGui::PopID();
				ImGui::PushID("Credit");
				ImGui::ImageButton(reinterpret_cast<ImTextureID>(packageIcon->getTextureId()), ImVec2(IconSize,IconSize));
				ImGui::PopID();
				ImGui::PushID("Quit");
				ImGui::ImageButton(reinterpret_cast<ImTextureID>(packageIcon->getTextureId()), ImVec2(IconSize,IconSize));
				ImGui::PopID();
				ImGui::EndPopup();
			}



			ImGui::PopStyleColor(1);
			ImGui::PopFont();
			ImGui::End();



		}
		if(getWindowIsShow(WindowType::ABOUT))
		{
			bool isOpen = true;
			if (ImGui::Begin(u8"关于", &isOpen, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Cube-Engine By Tzw.\ntzwtangziwen@163.com\nhttps://github.com/tangziwen/Cube-Engine");
				ImGui::Separator();
				if (ImGui::Button("OK", ImVec2(120, 0))) {isOpen = false; }
				ImGui::End();
			}
			setWindowShow(WindowType::ABOUT, isOpen);
		}
		if(getWindowIsShow(WindowType::Console))
		{
			bool isOpen = true;
			ConsolePanel::shared()->Draw("Console", &isOpen);
			setWindowShow(WindowType::Console, isOpen);
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
			auto editor = m_currControlPart->m_parent->getVehicle()->getEditor();
	        editor->drawIMGUI(&isOpen);
			setWindowShow(WindowType::NODE_EDITOR, isOpen);
		}

		if(getWindowIsShow(WindowType::INVENTORY))
		{
			bool isShow = ScriptPyMgr::shared()->callFunPyBool("tzw", "tzw_game_draw_window",int(WindowType::INVENTORY));
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
				GameWorld::shared()->saveGame("Data/PlayerData/Save/World.json");
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

		if(getWindowIsShow(WindowType::NEW_WORLD_SETTING))
		{

			bool isOpen = true;
	        m_NewWorldSettingUI->drawIMGUI(&isOpen);
			setWindowShow(WindowType::NEW_WORLD_SETTING, isOpen);
		}


		if(getWindowIsShow(WindowType::LOAD_WORLD))
		{
			bool isOpen = true;
	        m_loadWorldUI->drawIMGUI(&isOpen);
			setWindowShow(WindowType::LOAD_WORLD, isOpen);
		}
		//painter
		//喷漆部分
		if(getWindowIsShow(WindowType::PAINTER))
		{
			bool isOpen = true;
			m_painterUI->drawIMGUI(&isOpen);
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
		//World Setting
		if(getWindowIsShow(WindowType::WORLD_SETTING)) 
		{
			auto screenSize = Engine::shared()->winSize();
			ImGui::SetNextWindowPos(ImVec2(screenSize.x / 2.0, screenSize.y / 2.0), ImGuiCond_Appearing, ImVec2(0.5, 0.5));
			bool isOpen = true;
			ImGui::Begin(u8"世界设置",&isOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);

			ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
            if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) 
			{
        	auto flag = ImGuiTabItemFlags_None;

            if (ImGui::BeginTabItem(TRC(u8"Fog"), 0, flag)) 
			{
				auto fogMat = MaterialPool::shared()->getMaterialByName("GlobalFog");
				fogMat->inspect();
            	ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem(TRC(u8"Sun"), 0, flag)) 
			{
				auto dirLight = g_GetCurrScene()->getDirectionLight();
				float time = dirLight->getTime();
				ImGui::SliderFloat("Sun Time", &time, 0, 24);

				float longitude = TbaseMath::Radius2Ang(dirLight->longitude());
				ImGui::SliderFloat("Sun longitude", &longitude, 0, 360);
				dirLight->setTimeAndLongitude(time, TbaseMath::Ang2Radius(longitude));
				static char* items[] = { "Constant", "24X", "30X", "48X", "72X", "120X", "180X","240X","340X"};
		        if (ImGui::BeginCombo(TRC(u8"Time Elapse"), items[dirLight->getElapseLevel()], 0)) // The second parameter is the label previewed before opening the combo.
		        {
		            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		            {
		                bool is_selected = (dirLight->getElapseLevel() == n);
		                if (ImGui::Selectable(items[n], is_selected))
		                    dirLight->setElapseLevel(n);
		                if (is_selected)
		                    ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
		            }
		            ImGui::EndCombo();
		        }
				
				float sunIntensity = dirLight->intensity();
				ImGui::SliderFloat("SunIntensity", &sunIntensity, 0.0, 50.0);
				dirLight->setIntensity(sunIntensity);
            	ImGui::EndTabItem();
            }
            	ImGui::EndTabBar();
            }
			ImGui::End();
			setWindowShow(WindowType::WORLD_SETTING, isOpen);
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

	}else
	{
		if(GameWorld::shared()->getCurrentState() == GAME_STATE_RUNNING)
		{
			const float DISTANCE = 10.0f;
			static int corner = 0;
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
			ImGui::Text(" Demo Version");
			}
			ImGui::End();

			if(m_isOpenPlayerOverLay)
			{
				const float DISTANCE = 10.0f;
			    static int corner = 1;
			    ImGuiIO& io = ImGui::GetIO();
			    if (corner != -1)
			    {
			        ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
			        ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
			        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
			    }
			    ImGui::SetNextWindowBgAlpha(0.75f); // Transparent background
				ImGui::SetNextWindowSizeConstraints(ImVec2(210, -1), ImVec2(600, -1));
			    if (ImGui::Begin("PlayerOverLay OverLay", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
			    {

			    	ImGui::Text("FPS :%.1f", Engine::shared()->FPS());
			        ImGui::Text("Pos: %s", GameWorld::shared()->getPlayer()->getPos().getStr().c_str());
		    		if(!GameWorld::shared()->getPlayer()->camera()->getIsEnableGravity())
		    		{
		    			ImGui::TextColored(ImVec4(1.0,0.0,0.0,1.0), "Flying Mode('Space'-Up 'Ctrl'-Down)");
		    		} else
		    		{
		    			ImGui::Text("'X' To Activate Flying Mode");
		    		}

		    		if(BuildingSystem::shared()->isIsInXRayMode())
		    		{
		    			ImGui::TextColored(ImVec4(1,0,0,1.0), "XRay Mode On");
		    		} else
		    		{
		    			ImGui::Text("'T' To Activate XRay Mode");
		    		}

			    	ImGui::Text("'TAB' To Open Menu");
			    }
			    ImGui::End();
			}
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


void GameUISystem::openNodeEditor(GamePart* part)
{
	setWindowShow(WindowType::NODE_EDITOR, true);
	m_currControlPart = part;
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
	GUISystem::shared()->imguiUseLargeFont();
	auto screenSize = Engine::shared()->winSize();
	ImGui::SetNextWindowPos(ImVec2(screenSize.x / 2.0, screenSize.y / 2.0), ImGuiCond_Always, ImVec2(0.5, 0.5));
	if (ImGui::Begin("CubeEngine",0, ImGuiWindowFlags_NoMove| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse))
	{

		if(ImGui::Button(TRC("New Game"), ImVec2(160, 35)))
		{
			setWindowShow(WindowType::NEW_WORLD_SETTING, true);
		}
		ImGui::Spacing();
		if(ImGui::Button(TRC("Load"), ImVec2(160, 35)))
		{
			m_loadWorldUI->prepare();
			setWindowShow(WindowType::LOAD_WORLD, true);
		}
		ImGui::Spacing();
		if(ImGui::Button(TRC("Option"), ImVec2(160, 35)))
		{
			m_option.open();
			setWindowShow(WindowType::OPTION_MENU, true);
		}
		ImGui::Spacing();
		if(ImGui::Button(TRC("Help"), ImVec2(160, 35)))
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
		if(ImGui::Button(TRC("Exit"), ImVec2(160, 35))) 
		{
			TranslationMgr::shared()->dump();
			exit(0);
		}
		ImGui::Spacing();
		ImGui::End();
		ImGui::PopFont();
	}
}

void GameUISystem::openMainMenu()
{
	setWindowShow(WindowType::MainMenu, !GameUISystem::shared()->getWindowIsShow(WindowType::MainMenu));
}

LabelNew* GameUISystem::getCrossHairTipsInfo() const
{
	return m_crossHairTipsInfo;
}

void GameUISystem::initInGame()
{
    m_crossHair = Sprite::create("UITexture/cross_hair.png");
	 
    auto size = m_crossHair->getContentSize();
	
    m_crossHair->setPos2D(Engine::shared()->windowWidth()/2 - size.x/2,Engine::shared()->windowHeight()/2 - size.y/2);
	m_crossHair->setColor(vec4(1, 0.8, 0, 1));
    GameWorld::shared()->getMainRoot()->addChild(m_crossHair);
	m_crossHairTipsInfo = LabelNew::create(TRC("No Tips"));
	GameWorld::shared()->getMainRoot()->addChild(m_crossHairTipsInfo);
	m_crossHairTipsInfo->setIsVisible(false);
	m_crossHairTipsInfo->setPos2D(Engine::shared()->windowWidth()/2 - size.x/2,Engine::shared()->windowHeight()/2 - size.y/2 -35);
	// m_crossHair->addChild(m_crossHairTipsInfo);
}
} // namespace tzw
