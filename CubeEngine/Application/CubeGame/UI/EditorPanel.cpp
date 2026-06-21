#include "EditorPanel.h"
#include "CubeGame/BuildingSystem.h"
#include "CubeGame/Rail/RailSystem.h"
#include "EngineSrc/Game/EditorCamera.h"
#include "2D/GUISystem.h"

#include <cstdio>
#include <string>
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

	void EditorPanel::drawRailToolButton(const char* label, RailBuildMode mode, float width)
	{
		bool isSelected = m_railBuildMode == mode;
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
			m_railBuildMode = mode;
			if (RailSystem* railSystem = GameWorld::shared()->railSystem())
			{
				railSystem->setBuildMode(mode);
			}
		}

		ImGui::PopStyleColor(3);
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
		drawCameraModeWindow(screenSize);

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
				drawTrainTab();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem(u8"摆放物"))
			{
				m_activeTab = 2;
				if (RailSystem* railSystem = GameWorld::shared()->railSystem())
				{
					railSystem->clearLinePreview();
				}
				ImGui::TextUnformatted(u8"摆放物编辑功能占位");
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem(u8"植被"))
			{
				m_activeTab = 3;
				if (RailSystem* railSystem = GameWorld::shared()->railSystem())
				{
					railSystem->clearLinePreview();
				}
				ImGui::TextUnformatted(u8"植被编辑功能占位");
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::EndChild();

		ImGui::End();

		if (m_activeTab != 1)
		{
			if (RailSystem* railSystem = GameWorld::shared()->railSystem())
			{
				railSystem->lineManager().setAddModeLineId(InvalidRailLineId);
			}
		}

		drawRailFloatingWindows();
	}

	void EditorPanel::drawTerrainTab()
	{
		if (RailSystem* railSystem = GameWorld::shared()->railSystem())
		{
			railSystem->clearLinePreview();
		}

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

	void EditorPanel::drawTrainTab()
	{
		ImGui::Text(u8"Train Tool");
		ImGui::Separator();

		drawRailTrackPanel();

		ImGui::Separator();
		if (ImGui::Button(u8"线路面板", ImVec2(95.0f, 28.0f)))
		{
			m_lineWindowOpen = true;
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"车辆面板", ImVec2(95.0f, 28.0f)))
		{
			m_trainWindowOpen = true;
		}
	}

	void EditorPanel::drawRailTrackPanel()
	{
		RailSystem* railSystem = GameWorld::shared()->railSystem();
		if (railSystem && !m_lineWindowOpen)
		{
			railSystem->clearLinePreview();
		}

		float buttonWidth = 95.0f;
		drawRailToolButton(u8"增加铁轨", RailBuildMode::Add, buttonWidth);
		ImGui::SameLine();
		drawRailToolButton(u8"删除铁轨", RailBuildMode::Delete, buttonWidth);

		if (railSystem)
		{
			railSystem->setBuildMode(m_railBuildMode);
		}

		if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
		{
			if (railSystem)
			{
				railSystem->handlePrimaryClick(PlacementMode::CursorBased);
			}
		}

		if (ImGui::IsMouseClicked(1) && !ImGui::GetIO().WantCaptureMouse)
		{
			if (railSystem)
			{
				railSystem->handleSecondaryClick();
			}
		}
	}

	void EditorPanel::drawRailFloatingWindows()
	{
		if (m_lineWindowOpen)
		{
			ImGui::SetNextWindowSize(ImVec2(520.0f, 300.0f), ImGuiCond_FirstUseEver);
			if (ImGui::Begin(u8"线路面板", &m_lineWindowOpen, ImGuiWindowFlags_None))
			{
				drawRailLinePanel();
			}
			ImGui::End();
		}
		else if (RailSystem* railSystem = GameWorld::shared()->railSystem())
		{
			railSystem->clearLinePreview();
			railSystem->lineManager().setAddModeLineId(InvalidRailLineId);
		}

		if (m_trainWindowOpen)
		{
			ImGui::SetNextWindowSize(ImVec2(420.0f, 320.0f), ImGuiCond_FirstUseEver);
			if (ImGui::Begin(u8"车辆面板", &m_trainWindowOpen, ImGuiWindowFlags_None))
			{
				drawRailTrainPanel();
			}
			ImGui::End();
		}
	}

	void EditorPanel::drawRailLinePanel()
	{
		RailSystem* railSystem = GameWorld::shared()->railSystem();
		if (!railSystem)
		{
			ImGui::TextUnformatted(u8"RailSystem unavailable");
			return;
		}

		railSystem->setBuildMode(RailBuildMode::None);

		RailLineManager& lineManager = railSystem->lineManager();
		if (ImGui::Button(u8"创建线路"))
		{
			const RailLineId lineId = lineManager.createLine();
			lineManager.setSelectedLineId(lineId);
			railSystem->setLinePreview(lineId);
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"删除线路"))
		{
			const RailLineId lineId = lineManager.selectedLineId();
			if (lineId != InvalidRailLineId && lineManager.deleteLine(lineId))
			{
				railSystem->trainManager().clearLineAssignment(lineId);
				railSystem->clearLinePreview();
				if (lineManager.selectedLineId() != InvalidRailLineId)
				{
					railSystem->setLinePreview(lineManager.selectedLineId());
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"取消预览"))
		{
			lineManager.setSelectedLineId(InvalidRailLineId);
			lineManager.setAddModeLineId(InvalidRailLineId);
			railSystem->clearLinePreview();
		}

		ImGui::Separator();
		ImGui::Columns(2, "RailLineColumns", true);
		ImGui::SetColumnWidth(0, 160.0f);
		ImGui::TextUnformatted(u8"线路");
		ImGui::Separator();
		for (const RailLine& line : lineManager.lines())
		{
			const bool isSelected = line.id == lineManager.selectedLineId();
			const bool isAddMode = line.id == lineManager.addModeLineId();
			if (!line.isUsable)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.25f, 0.25f, 1.0f));
			}
			if (ImGui::Selectable((line.name + "##line").c_str(), isSelected, ImGuiSelectableFlags_None, ImVec2(90.0f, 0.0f)))
			{
				lineManager.setSelectedLineId(line.id);
				lineManager.setAddModeLineId(InvalidRailLineId);
				railSystem->setLinePreview(line.id);
			}
			if (isSelected)
			{
				ImGui::SameLine();
				const char* buttonText = isAddMode ? u8"完成##addToggle" : u8"添加##addToggle";
				if (ImGui::SmallButton(buttonText))
				{
					if (isAddMode)
					{
						lineManager.setAddModeLineId(InvalidRailLineId);
					}
					else
					{
						lineManager.setAddModeLineId(line.id);
					}
				}
			}
			if (!line.isUsable)
			{
				ImGui::PopStyleColor();
			}
		}

		ImGui::NextColumn();
		ImGui::TextUnformatted(u8"详情");
		ImGui::Separator();
		RailLine* selectedLine = lineManager.line(lineManager.selectedLineId());
		if (!selectedLine)
		{
			ImGui::TextUnformatted(u8"先创建或选择一条线路");
		}
		else
		{
			ImGui::Text("%s", selectedLine->name.c_str());
			if (selectedLine->isUsable)
			{
				ImGui::Text(u8"可用 %.1fm", selectedLine->totalLength);
			}
			else
			{
				ImGui::TextColored(ImVec4(1.0f, 0.25f, 0.25f, 1.0f), "%s", selectedLine->invalidReason.c_str());
			}

			for (int i = 0; i < static_cast<int>(selectedLine->controlNodes.size()); ++i)
			{
				char label[64];
				snprintf(label, sizeof(label), "Node %d##lineNode%d", selectedLine->controlNodes[i], i);
				if (ImGui::SmallButton(label))
				{
					if (lineManager.removeControlNodeAt(selectedLine->id, i, railSystem->network()))
					{
						railSystem->setLinePreview(selectedLine->id);
						railSystem->trainManager().refreshAfterLineRebuild(lineManager);
					}
					break;
				}
				ImGui::SameLine();
			}
		}
		ImGui::Columns(1, "RailLineColumns", false);

		if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
		{
			if (lineManager.addModeLineId() == lineManager.selectedLineId() && lineManager.addModeLineId() != InvalidRailLineId)
			{
				railSystem->addPickedNodeToSelectedLine(PlacementMode::CursorBased);
			}
		}
	}

	void EditorPanel::drawRailTrainPanel()
	{
		RailSystem* railSystem = GameWorld::shared()->railSystem();
		if (!railSystem)
		{
			ImGui::TextUnformatted(u8"RailSystem unavailable");
			return;
		}

		railSystem->setBuildMode(RailBuildMode::None);
		railSystem->clearLinePreview();

		if (ImGui::Button(u8"创建列车"))
		{
			ImGui::OpenPopup("CreateRailTrainPopup");
		}
		if (ImGui::BeginPopup("CreateRailTrainPopup"))
		{
			ImGui::SliderInt(u8"车厢数量", &m_newTrainCarriageCount, 0, 12);
			if (ImGui::Button(u8"创建"))
			{
				railSystem->trainManager().createTrain(m_newTrainCarriageCount);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		RailTrainId trainToDelete = InvalidRailTrainId;
		for (const RailTrain& train : railSystem->trainManager().trains())
		{
			ImGui::Separator();
			ImGui::Text("%s", train.name.c_str());
			ImGui::SameLine();
			ImGui::Text(u8"车厢 %d", train.carriageCount);
			ImGui::SameLine();
			std::string deleteButtonId = std::string(u8"删除##train") + std::to_string(train.id);
			if (ImGui::SmallButton(deleteButtonId.c_str()))
			{
				trainToDelete = train.id;
			}

			const RailLine* currentLine = railSystem->lineManager().line(train.lineId);
			const char* preview = currentLine ? currentLine->name.c_str() : u8"未分配";
			if (!currentLine)
			{
				if (train.pose.valid)
				{
					ImGui::TextUnformatted(u8"未分配线路，列车停在当前位置");
				}
				else
				{
					ImGui::TextUnformatted(u8"未分配线路，暂不显示在世界中");
				}
			}
			else if (!currentLine->isUsable)
			{
				ImGui::TextColored(ImVec4(1.0f, 0.25f, 0.25f, 1.0f), u8"线路不可用，列车停在最后有效位置");
			}
			std::string comboId = std::string(u8"线路##trainLine") + std::to_string(train.id);
			if (ImGui::BeginCombo(comboId.c_str(), preview))
			{
				for (const RailLine& line : railSystem->lineManager().lines())
				{
					if (!line.isUsable)
					{
						continue;
					}
					const bool isSelected = train.lineId == line.id;
					if (ImGui::Selectable(line.name.c_str(), isSelected))
					{
						railSystem->trainManager().assignLine(train.id, line.id, railSystem->lineManager());
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}
		if (trainToDelete != InvalidRailTrainId)
		{
			railSystem->trainManager().deleteTrain(trainToDelete);
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
