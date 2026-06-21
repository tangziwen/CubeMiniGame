#include "EditorPanel.h"
#include "CubeGame/BuildingSystem.h"
#include "2D/GUISystem.h"
namespace tzw
{

	void EditorPanel::drawToolButton(const char* label, ToolMode mode, float width)
	{
		bool isSelected = m_toolMode == mode;
		if (isSelected)
		{
			ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.5f, 0.8f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.3f, 0.6f, 0.9f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.2f, 0.5f, 0.8f, 1.0f));
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
		}

		if (ImGui::Selectable(label, isSelected, ImGuiSelectableFlags_None, ImVec2(width, 32.0f)))
		{
			m_toolMode = mode;
		}

		ImGui::PopStyleColor(3);
	}

	void EditorPanel::drawParameterPanel()
	{
		if (m_activeTab != 0)
		{
			ImGui::TextUnformatted(u8"当前分类无参数");
			return;
		}

		switch (m_toolMode)
		{
		case ToolMode::CarveSphere:
		case ToolMode::RaiseSphere:
			ImGui::SliderFloat("Radius", &m_radius, 0.1f, 10.0f, "%.1f");
			ImGui::SliderFloat("Strength", &m_strength, 0.0f, 1.0f, "%.2f");
			break;
		case ToolMode::CarveBox:
		case ToolMode::RaiseBox:
			ImGui::SliderFloat("Strength", &m_strength, 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("Half X", &m_boxHalfX, 0.1f, 10.0f, "%.1f");
			ImGui::SliderFloat("Half Y", &m_boxHalfY, 0.1f, 10.0f, "%.1f");
			ImGui::SliderFloat("Half Z", &m_boxHalfZ, 0.1f, 10.0f, "%.1f");
			break;
		case ToolMode::PaintSphere:
			ImGui::SliderFloat("Radius", &m_radius, 0.1f, 10.0f, "%.1f");
			ImGui::SliderInt("Material", &m_materialIndex, 0, 255);
			break;
		}
	}

	void EditorPanel::drawIMGUI(bool* isOpen)
	{
		vec2 screenSize = Engine::shared()->winSize();
		float yOffset = 15.0f;
		ImVec2 window_pos = ImVec2(screenSize.x / 2.0f, screenSize.y - yOffset);
		ImVec2 window_pos_pivot = ImVec2(0.5f, 1.0f);

		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		ImGui::SetNextWindowSize(ImVec2(520.0f, 200.0f), ImGuiCond_Always);
		ImGui::Begin("Editor", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

		// 左侧固定参数面板
		ImGui::BeginChild("ParameterPanel", ImVec2(220.0f, 0.0f), true, ImGuiWindowFlags_None);
		drawParameterPanel();
		ImGui::EndChild();

		ImGui::SameLine();

		// 右侧 Tab 区域
		ImGui::BeginChild("EditorTabs", ImVec2(0.0f, 0.0f), false, ImGuiWindowFlags_None);
		if (ImGui::BeginTabBar("EditorTabBar", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem(u8"地形"))
			{
				m_activeTab = 0;
				drawTerrainTab();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem(u8"火车"))
			{
				m_activeTab = 1;
				ImGui::TextUnformatted(u8"火车编辑功能占位");
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem(u8"摆放物"))
			{
				m_activeTab = 2;
				ImGui::TextUnformatted(u8"摆放物编辑功能占位");
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem(u8"植被"))
			{
				m_activeTab = 3;
				ImGui::TextUnformatted(u8"植被编辑功能占位");
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::EndChild();

		ImGui::End();
	}

	void EditorPanel::drawTerrainTab()
	{
		ImGui::Text("Terrain Tool");
		ImGui::Separator();

		float buttonWidth = 95.0f;
		drawToolButton("Carve Sphere", ToolMode::CarveSphere, buttonWidth);
		ImGui::SameLine();
		drawToolButton("Raise Sphere", ToolMode::RaiseSphere, buttonWidth);
		ImGui::SameLine();
		drawToolButton("Paint Sphere", ToolMode::PaintSphere, buttonWidth);

		drawToolButton("Carve Box", ToolMode::CarveBox, buttonWidth);
		ImGui::SameLine();
		drawToolButton("Raise Box", ToolMode::RaiseBox, buttonWidth);

		// 鼠标未悬停在任何 ImGui 窗口上时点击，触发地形编辑
		if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
		{
			applyTerrainEdit();
		}
	}

	void EditorPanel::applyTerrainEdit()
	{
		auto player = GameWorld::shared()->getPlayer();
		if (!player)
			return;

		switch (m_toolMode)
		{
		case ToolMode::CarveSphere:
			BuildingSystem::shared()->terrainCarveSphere(m_radius, m_strength, PlacementMode::CursorBased);
			break;
		case ToolMode::RaiseSphere:
			BuildingSystem::shared()->terrainRaiseSphere(m_radius, m_strength, PlacementMode::CursorBased);
			break;
		case ToolMode::CarveBox:
			BuildingSystem::shared()->terrainCarveBox(vec3(m_boxHalfX, m_boxHalfY, m_boxHalfZ), m_strength, PlacementMode::CursorBased);
			break;
		case ToolMode::RaiseBox:
			BuildingSystem::shared()->terrainRaiseBox(vec3(m_boxHalfX, m_boxHalfY, m_boxHalfZ), m_strength, PlacementMode::CursorBased);
			break;
		case ToolMode::PaintSphere:
			BuildingSystem::shared()->terrainPaint(m_materialIndex, m_radius, PlacementMode::CursorBased);
			break;
		}
	}

	EditorPanel::EditorPanel():m_onCreate(nullptr)
	{
	}

	void EditorPanel::prepare()
	{
	}

}
