#include "RailEditorModule.h"

#include "2D/IMGUISystem.h"
#include "2D/NotificationSystem.h"
#include "CubeGame/BuildingSystem.h"
#include "CubeGame/GameWorld.h"
#include "CubeGame/Rail/RailSystem.h"
#include "CubeGame/UI/InspectPanel.h"

#include <cstdio>
#include <string>

namespace tzw {

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

void notifyRailEditResult(RailEditResult result)
{
	if (result != RailEditResult::Success)
	{
		NotificationSystem::shared()->push("invalid operation", NotificationLevel::Warning);
	}
}

RailEditorState trackState(RailTrackAction action)
{
	RailEditorState state;
	state.domain = RailEditDomain::Track;
	state.trackAction = action;
	return state;
}

RailEditorState pointState(RailPointAction action)
{
	RailEditorState state;
	state.domain = RailEditDomain::Point;
	state.pointAction = action;
	return state;
}

RailEditorState lineState(RailLineAction action)
{
	RailEditorState state;
	state.domain = RailEditDomain::Line;
	state.lineAction = action;
	return state;
}
}

void RailEditorModule::drawMainTab(EditorModalState& state, InspectPanel& inspectPanel)
{
	ImGui::Text(u8"Train Tool");
	ImGui::Separator();

	drawRailTrackPanel(state);

	ImGui::Separator();
	float buttonWidth = 95.0f;
	drawRailToolButton(state, u8"创建站点", pointState(RailPointAction::AddStation), buttonWidth);
	ImGui::SameLine();
	drawRailToolButton(state, u8"删除站点", pointState(RailPointAction::DeleteStation), buttonWidth);
	drawRailToolButton(state, u8"创建路点", pointState(RailPointAction::AddRoutePoint), buttonWidth);
	ImGui::SameLine();
	drawRailToolButton(state, u8"删除路点", pointState(RailPointAction::DeleteRoutePoint), buttonWidth);

	ImGui::Separator();
	if (ImGui::Button(u8"线路面板", ImVec2(82.0f, 28.0f)))
	{
		m_lineWindowOpen = true;
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"车辆面板", ImVec2(82.0f, 28.0f)))
	{
		m_trainWindowOpen = true;
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"Inspect", ImVec2(78.0f, 28.0f)))
	{
		inspectPanel.open();
	}
}

void RailEditorModule::drawParameterPanel(EditorModalState& state)
{
	if (state.module != EditorModuleId::Rail || !state.rail.isActive())
	{
		ImGui::TextUnformatted(u8"未选择工具");
		return;
	}

	if (state.rail.isLineControlEditState())
	{
		drawLineEditParameterPanel(state, GameWorld::shared()->railSystem());
		return;
	}

	if (state.rail.isTrackEditState())
	{
		if (state.rail.trackAction == RailTrackAction::AddTrack)
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
	else if (state.rail.isPointEditState())
	{
		switch (state.rail.pointAction)
		{
		case RailPointAction::AddStation:
			ImGui::TextUnformatted(u8"创建站点");
			ImGui::TextWrapped(u8"左键点击已有轨道创建站点和默认月台。");
			break;
		case RailPointAction::DeleteStation:
			ImGui::TextUnformatted(u8"删除站点");
			ImGui::TextWrapped(u8"左键点击站点标记删除，不影响轨道。");
			break;
		case RailPointAction::AddRoutePoint:
			ImGui::TextUnformatted(u8"创建路点");
			ImGui::TextWrapped(u8"左键点击已有轨道创建线路导航路点。");
			break;
		case RailPointAction::DeleteRoutePoint:
			ImGui::TextUnformatted(u8"删除路点");
			ImGui::TextWrapped(u8"左键点击路点标记删除，不影响轨道。");
			break;
		case RailPointAction::None:
		default:
			break;
		}
	}
	else
	{
		ImGui::TextUnformatted(u8"当前分类无参数");
	}

	if (ImGui::Button(u8"退出编辑", ImVec2(-1.0f, 28.0f)))
	{
		state.clear();
	}
}

void RailEditorModule::drawFloatingWindows(EditorModalState& state, InspectPanel& inspectPanel)
{
	if (m_lineWindowOpen)
	{
		ImGui::SetNextWindowSize(ImVec2(520.0f, 300.0f), ImGuiCond_FirstUseEver);
		if (ImGui::Begin(u8"线路面板", &m_lineWindowOpen, ImGuiWindowFlags_None))
		{
			drawRailLinePanel(state);
		}
		ImGui::End();
	}
	if (m_trainWindowOpen)
	{
		ImGui::SetNextWindowSize(ImVec2(420.0f, 320.0f), ImGuiCond_FirstUseEver);
		if (ImGui::Begin(u8"车辆面板", &m_trainWindowOpen, ImGuiWindowFlags_None))
		{
			drawRailTrainPanel(inspectPanel);
		}
		ImGui::End();
	}
}

bool RailEditorModule::handlePrimaryClick(const EditorModalState& state, InspectPanel& inspectPanel)
{
	RailSystem* railSystem = GameWorld::shared()->railSystem();
	if (state.module != EditorModuleId::Rail || !state.rail.isActive())
	{
		return handleInspectPrimaryClick(railSystem, inspectPanel);
	}

	if (!railSystem)
	{
		return true;
	}

	switch (state.rail.domain)
	{
	case RailEditDomain::Track:
		if (state.rail.trackAction == RailTrackAction::AddTrack)
		{
			railSystem->handleTrackAddPrimaryClick(PlacementMode::CursorBased);
		}
		else if (state.rail.trackAction == RailTrackAction::DeleteTrack)
		{
			railSystem->handleTrackDeletePrimaryClick(PlacementMode::CursorBased);
		}
		return true;
	case RailEditDomain::Point:
		switch (state.rail.pointAction)
		{
		case RailPointAction::AddStation:
			notifyRailEditResult(railSystem->handleStationAddPrimaryClick(PlacementMode::CursorBased));
			return true;
		case RailPointAction::DeleteStation:
			railSystem->handleStationDeletePrimaryClick(PlacementMode::CursorBased);
			return true;
		case RailPointAction::AddRoutePoint:
			notifyRailEditResult(railSystem->handleRoutePointAddPrimaryClick(PlacementMode::CursorBased));
			return true;
		case RailPointAction::DeleteRoutePoint:
			railSystem->handleRoutePointDeletePrimaryClick(PlacementMode::CursorBased);
			return true;
		case RailPointAction::None:
		default:
			return false;
		}
	case RailEditDomain::Line:
		if (state.rail.lineAction == RailLineAction::AddControlPoint)
		{
			notifyRailEditResult(railSystem->addPickedControlPointToSelectedLine(PlacementMode::CursorBased));
			return true;
		}
		return false;
	case RailEditDomain::Train:
	case RailEditDomain::None:
	default:
		break;
	}

	return false;
}

bool RailEditorModule::handleSecondaryClick(const EditorModalState& state)
{
	RailSystem* railSystem = GameWorld::shared()->railSystem();
	if (railSystem && state.module == EditorModuleId::Rail && state.rail.isTrackEditState())
	{
		railSystem->cancelTrackEdit();
		return true;
	}
	return false;
}

void RailEditorModule::syncWorldVisuals(const EditorModalState& state)
{
	RailSystem* railSystem = GameWorld::shared()->railSystem();
	const RailEditorState railState = state.module == EditorModuleId::Rail ? state.rail : RailEditorState();
	if (!railSystem)
	{
		m_syncedRailState = railState;
		return;
	}

	if (m_syncedRailState != railState)
	{
		railSystem->cancelTrackEdit();
		railSystem->hideEditorVisuals();
		if (railState.isLineControlEditState())
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
		m_syncedRailState = railState;
	}

	switch (railState.domain)
	{
	case RailEditDomain::Track:
		if (railState.trackAction == RailTrackAction::AddTrack)
		{
			railSystem->syncTrackAddVisuals(PlacementMode::CursorBased);
		}
		else if (railState.trackAction == RailTrackAction::DeleteTrack)
		{
			railSystem->syncTrackDeleteVisuals();
		}
		else
		{
			railSystem->hideEditorVisuals();
		}
		break;
	case RailEditDomain::Point:
		switch (railState.pointAction)
		{
		case RailPointAction::DeleteStation:
			railSystem->showStationEditorVisuals(true);
			break;
		case RailPointAction::DeleteRoutePoint:
			railSystem->showRoutePointEditorVisuals(true);
			break;
		case RailPointAction::AddStation:
		case RailPointAction::AddRoutePoint:
		case RailPointAction::None:
		default:
			railSystem->hideEditorVisuals();
			break;
		}
		break;
	case RailEditDomain::Line:
		if (railState.lineAction == RailLineAction::AddControlPoint)
		{
			railSystem->showLineAddControlBillboards();
		}
		else if (railState.lineAction == RailLineAction::RemoveControlPoint)
		{
			railSystem->showLineRemoveControlBillboards();
		}
		else
		{
			railSystem->hideEditorVisuals();
		}
		break;
	case RailEditDomain::Train:
	case RailEditDomain::None:
	default:
		railSystem->hideEditorVisuals();
		break;
	}
}

void RailEditorModule::hideWorldVisuals()
{
	if (RailSystem* railSystem = GameWorld::shared()->railSystem())
	{
		railSystem->hideEditorVisuals();
	}
	m_syncedRailState = RailEditorState();
}

void RailEditorModule::drawRailTrackPanel(EditorModalState& state)
{
	float buttonWidth = 95.0f;
	drawRailToolButton(state, u8"增加铁轨", trackState(RailTrackAction::AddTrack), buttonWidth);
	ImGui::SameLine();
	drawRailToolButton(state, u8"删除铁轨", trackState(RailTrackAction::DeleteTrack), buttonWidth);
}

void RailEditorModule::drawRailLinePanel(EditorModalState& state)
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
	if (isLineControlEditState(state))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.45f);
	}
	if (ImGui::Button(u8"删除线路") && !isLineControlEditState(state))
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
	if (isLineControlEditState(state))
	{
		ImGui::PopStyleVar();
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"取消预览"))
	{
		lineManager.setSelectedLineId(InvalidRailLineId);
		if (isLineControlEditState(state))
		{
			state.clear();
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
				setRailState(state, lineState(RailLineAction::AddControlPoint));
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

void RailEditorModule::drawRailTrainPanel(InspectPanel& inspectPanel)
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
		ImGui::SameLine();
		std::string inspectButtonId = std::string(u8"查看##trainInspect") + std::to_string(train.id);
		if (ImGui::SmallButton(inspectButtonId.c_str()))
		{
			inspectPanel.inspect(RailInspectTarget::train(train.id), railSystem);
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

void RailEditorModule::drawLineEditParameterPanel(EditorModalState& state, RailSystem* railSystem)
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
			state.clear();
		}
		return;
	}

	ImGui::Text("%s", selectedLine->name.c_str());
	if (ImGui::RadioButton(u8"添加控制点", state.rail.lineAction == RailLineAction::AddControlPoint))
	{
		setRailState(state, lineState(RailLineAction::AddControlPoint));
	}
	ImGui::SameLine();
	if (ImGui::RadioButton(u8"删除控制点", state.rail.lineAction == RailLineAction::RemoveControlPoint))
	{
		setRailState(state, lineState(RailLineAction::RemoveControlPoint));
	}
	if (ImGui::Button(u8"退出线路编辑"))
	{
		state.clear();
		return;
	}

	ImGui::Separator();
	if (state.rail.lineAction == RailLineAction::AddControlPoint)
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

void RailEditorModule::drawRailToolButton(EditorModalState& state, const char* label, const RailEditorState& nextState, float width)
{
	bool isSelected = isRailStateSelected(state, nextState);
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
		if (isSelected)
		{
			state.clear();
		}
		else
		{
			setRailState(state, nextState);
		}
	}

	ImGui::PopStyleColor(3);
}

void RailEditorModule::setRailState(EditorModalState& state, const RailEditorState& railState)
{
	state.clear();
	state.module = EditorModuleId::Rail;
	state.rail = railState;
}

bool RailEditorModule::handleInspectPrimaryClick(RailSystem* railSystem, InspectPanel& inspectPanel)
{
	if (!railSystem)
	{
		return false;
	}

	RailInspectTarget target;
	if (!railSystem->pickInspectTarget(PlacementMode::CursorBased, target))
	{
		return false;
	}

	inspectPanel.inspect(target, railSystem);
	return true;
}

bool RailEditorModule::isLineControlEditState(const EditorModalState& state) const
{
	return state.module == EditorModuleId::Rail && state.rail.isLineControlEditState();
}

bool RailEditorModule::isRailStateSelected(const EditorModalState& state, const RailEditorState& railState) const
{
	return state.module == EditorModuleId::Rail && state.rail == railState;
}

} // namespace tzw
