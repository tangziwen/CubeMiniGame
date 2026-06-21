#include "EditorPanel.h"
#include "CubeGame/BuildingSystem.h"
#include "CubeGame/Rail/RailSystem.h"
#include "EngineSrc/Game/EditorCamera.h"
#include "2D/GUISystem.h"

#include <cstdio>
#include <string>
namespace tzw
{

	void EditorPanel::drawToolButton(const char* label, EditorState state, float width)
	{
		bool isSelected = m_editorState == state;
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
			setEditorState(state);
		}

		ImGui::PopStyleColor(3);
	}

	void EditorPanel::drawRailToolButton(const char* label, EditorState state, float width)
	{
		bool isSelected = m_editorState == state;
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
			setEditorState(state);
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

	void EditorPanel::setEditorState(EditorState state)
	{
		if (m_editorState == state)
		{
			return;
		}

		m_editorState = state;
		if (RailSystem* railSystem = GameWorld::shared()->railSystem())
		{
			railSystem->cancelTrackEdit();
			switch (m_editorState)
			{
			case EditorState::LineAddNode:
				if (railSystem->lineManager().selectedLineId() != InvalidRailLineId)
				{
					railSystem->setLinePreview(railSystem->lineManager().selectedLineId());
				}
				break;
			case EditorState::LineRemoveNode:
				if (railSystem->lineManager().selectedLineId() != InvalidRailLineId)
				{
					railSystem->setLinePreview(railSystem->lineManager().selectedLineId());
				}
				break;
			default:
				if (isTerrainState())
				{
					railSystem->clearLinePreview();
				}
				break;
			}
		}
	}

	bool EditorPanel::isLineNodeEditState() const
	{
		return m_editorState == EditorState::LineAddNode || m_editorState == EditorState::LineRemoveNode;
	}

	bool EditorPanel::isTerrainState() const
	{
		return m_editorState == EditorState::CarveSphere
			|| m_editorState == EditorState::RaiseSphere
			|| m_editorState == EditorState::CarveBox
			|| m_editorState == EditorState::RaiseBox
			|| m_editorState == EditorState::PaintSphere;
	}

	bool EditorPanel::isRailTrackState() const
	{
		return m_editorState == EditorState::TrackAdd || m_editorState == EditorState::TrackDelete;
	}

	void EditorPanel::drawParameterPanel()
	{
		if (isLineNodeEditState())
		{
			drawLineEditParameterPanel(GameWorld::shared()->railSystem());
			return;
		}

		if (isRailTrackState())
		{
			if (m_editorState == EditorState::TrackAdd)
			{
				ImGui::TextUnformatted(u8"轨道建造");
				ImGui::TextWrapped(u8"左键选择起点和终点，右键取消当前起点。");
			}
			else
			{
				ImGui::TextUnformatted(u8"轨道删除");
				ImGui::TextWrapped(u8"左键点击已有轨道段删除。");
			}
			return;
		}

		if (!isTerrainState())
		{
			ImGui::TextUnformatted(u8"当前分类无参数");
			return;
		}

		switch (m_editorState)
		{
		case EditorState::CarveSphere:
		case EditorState::RaiseSphere:
			ImGui::SliderFloat("Radius", &m_radius, 0.1f, 10.0f, "%.1f");
			ImGui::SliderFloat("Strength", &m_strength, 0.0f, 1.0f, "%.2f");
			break;
		case EditorState::CarveBox:
		case EditorState::RaiseBox:
			ImGui::SliderFloat("Strength", &m_strength, 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("Half X", &m_boxHalfX, 0.1f, 10.0f, "%.1f");
			ImGui::SliderFloat("Half Y", &m_boxHalfY, 0.1f, 10.0f, "%.1f");
			ImGui::SliderFloat("Half Z", &m_boxHalfZ, 0.1f, 10.0f, "%.1f");
			break;
		case EditorState::PaintSphere:
			ImGui::SliderFloat("Radius", &m_radius, 0.1f, 10.0f, "%.1f");
			ImGui::SliderInt("Material", &m_materialIndex, 0, 255);
			break;
		default:
			break;
		}
	}

	void EditorPanel::drawLineEditParameterPanel(RailSystem* railSystem)
	{
		if (!railSystem)
		{
			ImGui::TextUnformatted(u8"RailSystem unavailable");
			return;
		}

		RailLineManager& lineManager = railSystem->lineManager();
		RailLine* selectedLine = lineManager.line(lineManager.selectedLineId());
		if (!selectedLine)
		{
			ImGui::TextUnformatted(u8"先选择线路");
			if (ImGui::Button(u8"退出线路编辑"))
			{
				setEditorState(EditorState::TrackAdd);
			}
			return;
		}

		ImGui::Text("%s", selectedLine->name.c_str());
		if (ImGui::RadioButton(u8"添加节点", m_editorState == EditorState::LineAddNode))
		{
			setEditorState(EditorState::LineAddNode);
		}
		ImGui::SameLine();
		if (ImGui::RadioButton(u8"删除节点", m_editorState == EditorState::LineRemoveNode))
		{
			setEditorState(EditorState::LineRemoveNode);
		}
		if (ImGui::Button(u8"退出线路编辑"))
		{
			setEditorState(EditorState::TrackAdd);
			return;
		}

		ImGui::Separator();
		if (selectedLine->controlNodes.empty())
		{
			ImGui::TextUnformatted(u8"当前线路没有节点");
			return;
		}

		ImGui::TextUnformatted(u8"线路节点");
		for (int i = 0; i < static_cast<int>(selectedLine->controlNodes.size()); ++i)
		{
			char label[64];
			snprintf(label, sizeof(label), "Node %d", selectedLine->controlNodes[i]);
			ImGui::Text("%s", label);
			ImGui::SameLine();

			char buttonId[64];
			snprintf(buttonId, sizeof(buttonId), u8"删除##lineNodeRemove%d", i);
			if (ImGui::SmallButton(buttonId))
			{
				if (lineManager.removeControlNodeAt(selectedLine->id, i, railSystem->network()))
				{
					railSystem->setLinePreview(selectedLine->id);
					railSystem->trainManager().refreshAfterLineRebuild(lineManager);
				}
				break;
			}
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
				if (m_activeTab != 0 && !isTerrainState())
				{
					setEditorState(EditorState::CarveSphere);
				}
				m_activeTab = 0;
				drawTerrainTab();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem(u8"火车"))
			{
				if (m_activeTab != 1 && !isRailTrackState() && !isLineNodeEditState())
				{
					setEditorState(EditorState::TrackAdd);
				}
				m_activeTab = 1;
				drawTrainTab();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem(u8"摆放物"))
			{
				if (m_activeTab != 2)
				{
					setEditorState(EditorState::None);
				}
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
				if (m_activeTab != 3)
				{
					setEditorState(EditorState::None);
				}
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

		drawRailFloatingWindows();
		handleEditorWorldInput();
	}

	void EditorPanel::handleEditorWorldInput()
	{
		RailSystem* railSystem = GameWorld::shared()->railSystem();
		if (railSystem)
		{
			switch (m_editorState)
			{
			case EditorState::TrackAdd:
				railSystem->syncTrackAddVisuals(PlacementMode::CursorBased);
				break;
			case EditorState::TrackDelete:
				railSystem->syncTrackDeleteVisuals();
				break;
			case EditorState::LineAddNode:
				railSystem->syncLineAddNodeVisuals();
				break;
			case EditorState::LineRemoveNode:
				railSystem->syncLineRemoveNodeVisuals();
				break;
			default:
				railSystem->hideEditorVisuals();
				break;
			}
		}

		if (ImGui::GetIO().WantCaptureMouse)
		{
			return;
		}

		if (ImGui::IsMouseClicked(0))
		{
			switch (m_editorState)
			{
			case EditorState::CarveSphere:
			case EditorState::RaiseSphere:
			case EditorState::CarveBox:
			case EditorState::RaiseBox:
			case EditorState::PaintSphere:
				applyTerrainEdit();
				break;
			case EditorState::TrackAdd:
				if (railSystem)
				{
					railSystem->handleTrackAddPrimaryClick(PlacementMode::CursorBased);
				}
				break;
			case EditorState::TrackDelete:
				if (railSystem)
				{
					railSystem->handleTrackDeletePrimaryClick(PlacementMode::CursorBased);
				}
				break;
			case EditorState::LineAddNode:
				if (railSystem)
				{
					railSystem->addPickedNodeToSelectedLine(PlacementMode::CursorBased);
				}
				break;
			case EditorState::LineRemoveNode:
				if (railSystem)
				{
					railSystem->removePickedNodeFromSelectedLine(PlacementMode::CursorBased);
				}
				break;
			default:
				break;
			}
		}

		if (ImGui::IsMouseClicked(1) && railSystem && isRailTrackState())
		{
			railSystem->cancelTrackEdit();
		}
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
		drawToolButton("Carve Sphere", EditorState::CarveSphere, buttonWidth);
		ImGui::SameLine();
		drawToolButton("Raise Sphere", EditorState::RaiseSphere, buttonWidth);
		ImGui::SameLine();
		drawToolButton("Paint Sphere", EditorState::PaintSphere, buttonWidth);

		drawToolButton("Carve Box", EditorState::CarveBox, buttonWidth);
		ImGui::SameLine();
		drawToolButton("Raise Box", EditorState::RaiseBox, buttonWidth);
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
		drawRailToolButton(u8"增加铁轨", EditorState::TrackAdd, buttonWidth);
		ImGui::SameLine();
		drawRailToolButton(u8"删除铁轨", EditorState::TrackDelete, buttonWidth);
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
			if (isLineNodeEditState())
			{
				setEditorState(EditorState::TrackAdd);
			}
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

		RailLineManager& lineManager = railSystem->lineManager();
		if (ImGui::Button(u8"创建线路"))
		{
			const RailLineId lineId = lineManager.createLine();
			lineManager.setSelectedLineId(lineId);
			railSystem->setLinePreview(lineId);
		}
		ImGui::SameLine();
		if (isLineNodeEditState())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.45f);
		}
		if (ImGui::Button(u8"删除线路") && !isLineNodeEditState())
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
		if (isLineNodeEditState())
		{
			ImGui::PopStyleVar();
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"取消预览"))
		{
			lineManager.setSelectedLineId(InvalidRailLineId);
			if (isLineNodeEditState())
			{
				setEditorState(EditorState::TrackAdd);
			}
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
			if (!line.isUsable)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.25f, 0.25f, 1.0f));
			}
			if (ImGui::Selectable((line.name + "##line").c_str(), isSelected, ImGuiSelectableFlags_None, ImVec2(90.0f, 0.0f)))
			{
				lineManager.setSelectedLineId(line.id);
				railSystem->setLinePreview(line.id);
			}
			if (isSelected)
			{
				ImGui::SameLine();
				if (ImGui::SmallButton(u8"编辑##lineEdit"))
				{
					setEditorState(EditorState::LineAddNode);
					railSystem->setLinePreview(line.id);
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
				ImGui::TextUnformatted(label);
				ImGui::SameLine();
			}
		}
		ImGui::Columns(1, "RailLineColumns", false);
	}

	void EditorPanel::drawRailTrainPanel()
	{
		RailSystem* railSystem = GameWorld::shared()->railSystem();
		if (!railSystem)
		{
			ImGui::TextUnformatted(u8"RailSystem unavailable");
			return;
		}

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

		switch (m_editorState)
		{
		case EditorState::CarveSphere:
			BuildingSystem::shared()->terrainCarveSphere(m_radius, m_strength, PlacementMode::CursorBased);
			break;
		case EditorState::RaiseSphere:
			BuildingSystem::shared()->terrainRaiseSphere(m_radius, m_strength, PlacementMode::CursorBased);
			break;
		case EditorState::CarveBox:
			BuildingSystem::shared()->terrainCarveBox(vec3(m_boxHalfX, m_boxHalfY, m_boxHalfZ), m_strength, PlacementMode::CursorBased);
			break;
		case EditorState::RaiseBox:
			BuildingSystem::shared()->terrainRaiseBox(vec3(m_boxHalfX, m_boxHalfY, m_boxHalfZ), m_strength, PlacementMode::CursorBased);
			break;
		case EditorState::PaintSphere:
			BuildingSystem::shared()->terrainPaint(m_materialIndex, m_radius, PlacementMode::CursorBased);
			break;
		default:
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
