#include "EditorPanel.h"
#include "CubeGame/BuildingSystem.h"
#include "CubeGame/GameState.h"
#include "CubeGame/Rail/RailSystem.h"
#include "EngineSrc/Game/EditorCamera.h"
#include "2D/IMGUISystem.h"
#include "Event/EventMgr.h"

#include <cstdio>
#include <string>
namespace tzw
{
	namespace
	{
		std::string controlPointLabel(const RailSystem* railSystem, const RailLineControlPoint& controlPoint)
		{
			if (!railSystem)
			{
				return "Unknown";
			}

			if (controlPoint.kind == RailLineControlPointKind::Station)
			{
				const RailStation* station = railSystem->stationManager().station(controlPoint.stationId);
				return station ? station->name : "Missing Station";
			}

			const RailRoutePoint* routePoint = railSystem->routePointManager().routePoint(controlPoint.routePointId);
			return routePoint ? routePoint->name : "Missing Route Point";
		}
	}

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
			if (m_editorState == state)
			{
				setEditorState(EditorState::None);
			}
			else
			{
				setEditorState(state);
			}
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
			if (m_editorState == state)
			{
				setEditorState(EditorState::None);
			}
			else
			{
				setEditorState(state);
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

	void EditorPanel::setEditorState(EditorState state)
	{
		if (m_editorState == state)
		{
			return;
		}

		m_editorState = state;
	}

	bool EditorPanel::isLineControlEditState() const
	{
		return m_editorState == EditorState::LineAddControlPoint
			|| m_editorState == EditorState::LineRemoveControlPoint;
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

	bool EditorPanel::isStationEditState() const
	{
		return m_editorState == EditorState::StationAdd || m_editorState == EditorState::StationDelete;
	}

	bool EditorPanel::isRoutePointEditState() const
	{
		return m_editorState == EditorState::RoutePointAdd || m_editorState == EditorState::RoutePointDelete;
	}

	void EditorPanel::drawParameterPanel()
	{
		if (m_editorState == EditorState::None)
		{
			ImGui::TextUnformatted(u8"未选择工具");
		}
		else if (isLineControlEditState())
		{
			drawLineEditParameterPanel(GameWorld::shared()->railSystem());
		}
		else if (isRailTrackState())
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
		}
		else if (isStationEditState())
		{
			if (m_editorState == EditorState::StationAdd)
			{
				ImGui::TextUnformatted(u8"创建站点");
				ImGui::TextWrapped(u8"左键点击已有轨道创建站点和默认月台。");
			}
			else
			{
				ImGui::TextUnformatted(u8"删除站点");
				ImGui::TextWrapped(u8"左键点击站点标记删除，不影响轨道。");
			}
		}
		else if (isRoutePointEditState())
		{
			if (m_editorState == EditorState::RoutePointAdd)
			{
				ImGui::TextUnformatted(u8"创建路点");
				ImGui::TextWrapped(u8"左键点击已有轨道创建线路导航路点。");
			}
			else
			{
				ImGui::TextUnformatted(u8"删除路点");
				ImGui::TextWrapped(u8"左键点击路点标记删除，不影响轨道。");
			}
		}
		else if (!isTerrainState())
		{
			ImGui::TextUnformatted(u8"当前分类无参数");
		}
		else
		{
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

		if (!isLineControlEditState())
		{
			if (m_editorState != EditorState::None)
			{
				if (ImGui::Button(u8"退出编辑", ImVec2(-1.0f, 28.0f)))
				{
					setEditorState(EditorState::None);
				}
			}
		}
	}

	void EditorPanel::drawParameterWindow(vec2 screenSize)
	{
		ImGui::SetNextWindowPos(
			ImVec2(15.0f, screenSize.y - 80.0f),
			ImGuiCond_Always,
			ImVec2(0.0f, 1.0f));
		ImGui::SetNextWindowSize(ImVec2(220.0f, 180.0f), ImGuiCond_Always);
		ImGui::Begin("EditorParameter", nullptr,
			ImGuiWindowFlags_NoDecoration
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoSavedSettings);
		drawParameterPanel();
		ImGui::End();
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
				setEditorState(EditorState::None);
			}
			return;
		}

		ImGui::Text("%s", selectedLine->name.c_str());
		if (ImGui::RadioButton(u8"添加控制点", m_editorState == EditorState::LineAddControlPoint))
		{
			setEditorState(EditorState::LineAddControlPoint);
		}
		ImGui::SameLine();
		if (ImGui::RadioButton(u8"删除控制点", m_editorState == EditorState::LineRemoveControlPoint))
		{
			setEditorState(EditorState::LineRemoveControlPoint);
		}
		if (ImGui::Button(u8"退出线路编辑"))
		{
			setEditorState(EditorState::None);
			return;
		}

		ImGui::Separator();
		if (m_editorState == EditorState::LineAddControlPoint)
		{
			ImGui::TextWrapped(u8"左键点击世界中的站点或路点，或在线路面板列表中添加。");
		}
		if (selectedLine->controlPoints.empty())
		{
			ImGui::TextUnformatted(u8"当前线路没有控制点");
			return;
		}

		ImGui::TextUnformatted(u8"线路控制点");
		for (int i = 0; i < static_cast<int>(selectedLine->controlPoints.size()); ++i)
		{
			std::string label = controlPointLabel(railSystem, selectedLine->controlPoints[i]);
			ImGui::Text("%d. %s", i + 1, label.c_str());
			ImGui::SameLine();

			char buttonId[64];
			snprintf(buttonId, sizeof(buttonId), u8"删除##lineControlRemove%d", i);
			if (ImGui::SmallButton(buttonId))
			{
				if (lineManager.removeControlPointAt(selectedLine->id, i, railSystem->network(),
					railSystem->anchorManager(), railSystem->stationManager(), railSystem->routePointManager()))
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
				if (m_activeTab != 0 && !isTerrainState())
				{
					setEditorState(EditorState::None);
				}
				m_activeTab = 0;
				drawTerrainTab();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem(u8"火车"))
			{
				if (m_activeTab != 1 && !isRailTrackState() && !isStationEditState()
					&& !isRoutePointEditState() && !isLineControlEditState())
				{
					setEditorState(EditorState::None);
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
				ImGui::TextUnformatted(u8"植被编辑功能占位");
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}

		ImGui::End();

		drawRailFloatingWindows();
	}

	void EditorPanel::onFrameUpdate(float)
	{
		if (GameState::shared()->getPlayerMode() == PlayerMode::Editor)
		{
			syncEditorWorldVisuals();
			return;
		}

		if (RailSystem* railSystem = GameWorld::shared()->railSystem())
		{
			railSystem->hideEditorVisuals();
		}
	}

	void EditorPanel::syncEditorWorldVisuals()
	{
		RailSystem* railSystem = GameWorld::shared()->railSystem();
		if (railSystem)
		{
			if (m_syncedEditorState != m_editorState)
			{
				railSystem->cancelTrackEdit();
				railSystem->hideEditorVisuals();
				if (isLineControlEditState())
				{
					const RailLineId selectedLineId = railSystem->lineManager().selectedLineId();
					if (selectedLineId != InvalidRailLineId)
					{
						railSystem->setLinePreview(selectedLineId);
					}
				}
				else
				{
					railSystem->clearLinePreview();
				}
				m_syncedEditorState = m_editorState;
			}

			switch (m_editorState)
			{
			case EditorState::TrackAdd:
				railSystem->syncTrackAddVisuals(PlacementMode::CursorBased);
				break;
			case EditorState::TrackDelete:
				railSystem->syncTrackDeleteVisuals();
				break;
			case EditorState::StationAdd:
				railSystem->hideEditorVisuals();
				break;
			case EditorState::StationDelete:
				railSystem->showStationEditorVisuals(true);
				break;
			case EditorState::RoutePointAdd:
				railSystem->hideEditorVisuals();
				break;
			case EditorState::RoutePointDelete:
				railSystem->showRoutePointEditorVisuals(true);
				break;
			case EditorState::LineAddControlPoint:
				railSystem->showLineAddControlBillboards();
				break;
			case EditorState::LineRemoveControlPoint:
				railSystem->showLineRemoveControlBillboards();
				break;
			default:
				railSystem->hideEditorVisuals();
				break;
			}
		}

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
			return handleEditorPrimaryClick();
		}
		if (button == TZW_MOUSE_BUTTON_RIGHT)
		{
			return handleEditorSecondaryClick();
		}
		return false;
	}

	bool EditorPanel::handleEditorPrimaryClick()
	{
		RailSystem* railSystem = GameWorld::shared()->railSystem();
		switch (m_editorState)
		{
		case EditorState::CarveSphere:
		case EditorState::RaiseSphere:
		case EditorState::CarveBox:
		case EditorState::RaiseBox:
		case EditorState::PaintSphere:
			applyTerrainEdit();
			return true;
		case EditorState::TrackAdd:
			if (railSystem)
			{
				railSystem->handleTrackAddPrimaryClick(PlacementMode::CursorBased);
			}
			return true;
		case EditorState::TrackDelete:
			if (railSystem)
			{
				railSystem->handleTrackDeletePrimaryClick(PlacementMode::CursorBased);
			}
			return true;
		case EditorState::StationAdd:
			if (railSystem)
			{
				railSystem->handleStationAddPrimaryClick(PlacementMode::CursorBased);
			}
			return true;
		case EditorState::StationDelete:
			if (railSystem)
			{
				railSystem->handleStationDeletePrimaryClick(PlacementMode::CursorBased);
			}
			return true;
		case EditorState::RoutePointAdd:
			if (railSystem)
			{
				railSystem->handleRoutePointAddPrimaryClick(PlacementMode::CursorBased);
			}
			return true;
		case EditorState::RoutePointDelete:
			if (railSystem)
			{
				railSystem->handleRoutePointDeletePrimaryClick(PlacementMode::CursorBased);
			}
			return true;
		case EditorState::LineAddControlPoint:
			if (railSystem)
			{
				railSystem->addPickedControlPointToSelectedLine(PlacementMode::CursorBased);
			}
			return true;
		default:
			break;
		}
		return false;
	}

	bool EditorPanel::handleEditorSecondaryClick()
	{
		RailSystem* railSystem = GameWorld::shared()->railSystem();
		if (railSystem && isRailTrackState())
		{
			railSystem->cancelTrackEdit();
			return true;
		}
		return false;
	}

	void EditorPanel::drawTerrainTab()
	{
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
		float buttonWidth = 95.0f;
		drawRailToolButton(u8"创建站点", EditorState::StationAdd, buttonWidth);
		ImGui::SameLine();
		drawRailToolButton(u8"删除站点", EditorState::StationDelete, buttonWidth);
		drawRailToolButton(u8"创建路点", EditorState::RoutePointAdd, buttonWidth);
		ImGui::SameLine();
		drawRailToolButton(u8"删除路点", EditorState::RoutePointDelete, buttonWidth);

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
		if (isLineControlEditState())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.45f);
		}
		if (ImGui::Button(u8"删除线路") && !isLineControlEditState())
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
		if (isLineControlEditState())
		{
			ImGui::PopStyleVar();
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"取消预览"))
		{
			lineManager.setSelectedLineId(InvalidRailLineId);
			if (isLineControlEditState())
			{
				setEditorState(EditorState::None);
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
					setEditorState(EditorState::LineAddControlPoint);
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

			ImGui::TextUnformatted(u8"控制点");
			for (int i = 0; i < static_cast<int>(selectedLine->controlPoints.size()); ++i)
			{
				std::string label = controlPointLabel(railSystem, selectedLine->controlPoints[i]);
				ImGui::Text("%d. %s", i + 1, label.c_str());
			}

			ImGui::Separator();
			ImGui::TextUnformatted(u8"添加控制点");
			for (const RailStation& station : railSystem->stationManager().stations())
			{
				std::string buttonId = station.name + "##addStationToLine" + std::to_string(station.id);
				if (ImGui::SmallButton(buttonId.c_str()))
				{
					railSystem->addStationToSelectedLine(station.id);
					railSystem->setLinePreview(selectedLine->id);
				}
				ImGui::SameLine();
			}
			for (const RailRoutePoint& routePoint : railSystem->routePointManager().routePoints())
			{
				std::string buttonId = routePoint.name + "##addRoutePointToLine" + std::to_string(routePoint.id);
				if (ImGui::SmallButton(buttonId.c_str()))
				{
					railSystem->addRoutePointToSelectedLine(routePoint.id);
					railSystem->setLinePreview(selectedLine->id);
				}
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
		RailTrainId trainToToggle = InvalidRailTrainId;
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
			ImGui::SameLine();
			std::string toggleButtonId = std::string(train.isManuallyStopped ? u8"运行##trainRun" : u8"停车##trainRun")
				+ std::to_string(train.id);
			if (ImGui::SmallButton(toggleButtonId.c_str()))
			{
				trainToToggle = train.id;
			}

			const RailLine* currentLine = railSystem->lineManager().line(train.lineId);
			const char* preview = currentLine ? currentLine->name.c_str() : u8"未分配";
			if (train.isManuallyStopped)
			{
				ImGui::TextUnformatted(u8"手动停车");
			}
			else if (train.isDwelling)
			{
				ImGui::Text(u8"到站停靠 %.1fs", train.dwellRemainingSeconds);
			}
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
		if (trainToToggle != InvalidRailTrainId)
		{
			railSystem->trainManager().toggleTrainRunning(trainToToggle);
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
		setFixedPiority(100);
		EventMgr::shared()->addFixedPiorityListener(this);
	}

	void EditorPanel::prepare()
	{
	}

}
