#include "EditorPanel.h"

#include "2D/IMGUISystem.h"
#include "CubeGame/GameState.h"
#include "CubeGame/GameWorld.h"
#include "EngineSrc/Game/EditorCamera.h"
#include "Event/EventMgr.h"

namespace tzw
{
	namespace
	{
		const int TerrainTabIndex = 0;
		const int RailTabIndex = 1;
		const int PlacementTabIndex = 2;
		const int VegetationTabIndex = 3;
	}

	void EditorPanel::drawCameraModeCombo()
	{
		auto player = GameWorld::shared()->getPlayer();
		EditorCamera* camera = player ? player->editorCamera() : nullptr;
		if (!camera)
		{
			ImGui::TextUnformatted("Camera Mode");
			ImGui::TextUnformatted("No editor camera");
			return;
		}

		const char* items[] = { "Free", "City Builder" };
		int currentItem = camera->mode() == EditorCamera::Mode::CityBuilder ? 1 : 0;
		if (ImGui::BeginCombo("Camera Mode", items[currentItem], 0))
		{
			for (int n = 0; n < 2; ++n)
			{
				const bool isSelected = currentItem == n;
				if (ImGui::Selectable(items[n], isSelected))
				{
					currentItem = n;
					camera->setMode(n == 1 ? EditorCamera::Mode::CityBuilder : EditorCamera::Mode::Free);
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}

	void EditorPanel::drawCameraModeWindow(vec2 screenSize)
	{
		ImGui::SetNextWindowPos(
			ImVec2(15.0f, screenSize.y - 15.0f),
			ImGuiCond_Always,
			ImVec2(0.0f, 1.0f));
		ImGui::SetNextWindowSize(ImVec2(220.0f, 58.0f), ImGuiCond_Always);
		ImGui::Begin("EditorCameraMode", nullptr,
			ImGuiWindowFlags_NoDecoration
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoSavedSettings);
		drawCameraModeCombo();
		ImGui::End();
	}

	void EditorPanel::drawParameterPanel()
	{
		if (m_modalState.module == EditorModuleId::Terrain)
		{
			m_terrainModule.drawParameterPanel(m_modalState);
		}
		else if (m_modalState.module == EditorModuleId::Rail)
		{
			m_railModule.drawParameterPanel(m_modalState);
		}
		else
		{
			ImGui::TextUnformatted(u8"未选择工具");
		}
	}

	void EditorPanel::drawParameterWindow(vec2 screenSize)
	{
		ImGui::SetNextWindowPos(
			ImVec2(15.0f, screenSize.y - 96.0f),
			ImGuiCond_Always,
			ImVec2(0.0f, 1.0f));
		ImGui::SetNextWindowSize(ImVec2(300.0f, 260.0f), ImGuiCond_Always);
		ImGui::Begin("EditorParameter", nullptr,
			ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoSavedSettings);
		drawParameterPanel();
		ImGui::End();
	}

	void EditorPanel::drawIMGUI(bool* isOpen)
	{
		(void)isOpen;
		vec2 screenSize = Engine::shared()->winSize();
		drawCameraModeWindow(screenSize);
		drawParameterWindow(screenSize);

		float yOffset = 15.0f;
		ImVec2 window_pos = ImVec2(screenSize.x / 2.0f, screenSize.y - yOffset);
		ImVec2 window_pos_pivot = ImVec2(0.5f, 1.0f);

		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		ImGui::SetNextWindowSize(ImVec2(280.0f, 200.0f), ImGuiCond_Always);
		ImGui::Begin("Editor", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

		if (ImGui::BeginTabBar("EditorTabBar", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem(u8"地形"))
			{
				switchToTab(TerrainTabIndex, EditorModuleId::Terrain);
				m_terrainModule.drawMainTab(m_modalState);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem(u8"火车"))
			{
				switchToTab(RailTabIndex, EditorModuleId::Rail);
				m_railModule.drawMainTab(m_modalState, m_inspectPanel);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem(u8"摆放物"))
			{
				switchToTab(PlacementTabIndex, EditorModuleId::Placement);
				ImGui::TextUnformatted(u8"摆放物编辑功能占位");
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem(u8"植被"))
			{
				switchToTab(VegetationTabIndex, EditorModuleId::None);
				ImGui::TextUnformatted(u8"植被编辑功能占位");
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}

		ImGui::End();

		m_railModule.drawFloatingWindows(m_modalState, m_inspectPanel);
		m_inspectPanel.draw(GameWorld::shared()->railSystem());
	}

	void EditorPanel::onFrameUpdate(float)
	{
		if (GameState::shared()->getPlayerMode() == PlayerMode::Editor)
		{
			m_railModule.syncWorldVisuals(m_modalState);
			return;
		}

		m_railModule.hideWorldVisuals();
	}

	bool EditorPanel::onMousePress(int button, vec2 pos)
	{
		(void)pos;
		if (GameState::shared()->getPlayerMode() != PlayerMode::Editor)
		{
			return false;
		}
		if (button == TZW_MOUSE_BUTTON_LEFT)
		{
			if (m_terrainModule.handlePrimaryClick(m_modalState))
			{
				return true;
			}
			return m_railModule.handlePrimaryClick(m_modalState, m_inspectPanel);
		}
		if (button == TZW_MOUSE_BUTTON_RIGHT)
		{
			return m_railModule.handleSecondaryClick(m_modalState);
		}
		return false;
	}

	void EditorPanel::switchToTab(int tabIndex, EditorModuleId moduleId)
	{
		if (m_activeTab == tabIndex)
		{
			return;
		}

		if (m_modalState.module != moduleId)
		{
			m_modalState.clear();
		}
		m_activeTab = tabIndex;
	}

	EditorPanel::EditorPanel():m_onCreate(nullptr)
	{
		setStandaloneEventPriority(100);
		EventMgr::shared()->addStandaloneEventListener(this);
	}

	void EditorPanel::prepare()
	{
	}

}
