#include "RailEditorImGuiView.h"

#include "2D/IMGUISystem.h"
#include "CubeGame/Editor/RailEditingController.h"
#include "CubeGame/GameWorld.h"
#include "CubeGame/Rail/RailLineVisualStyle.h"
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

ImVec4 imguiColor(const vec4& color)
{
	return ImVec4(color.x, color.y, color.z, color.w);
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

void RailEditorImGuiView::drawMainTab(EditorState& state, RailEditingController& controller, InspectPanel& inspectPanel)
{
	if (ImGui::BeginTabBar("RailEditorToolTabs", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem(u8"建造"))
		{
			drawRailTrackPanel(state, controller);
			ImGui::Separator();
			drawRailPointPanel(state, controller);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(u8"线路"))
		{
			drawRailManagementPanel(inspectPanel);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

void RailEditorImGuiView::drawParameterPanel(EditorState& state, RailEditingController& controller)
{
	if (state.module != EditorModuleId::Rail || !state.rail.isActive())
	{
		ImGui::TextUnformatted(u8"未选择工具");
		return;
	}

	if (state.rail.isLineControlEditState())
	{
		drawLineEditParameterPanel(state, controller, GameWorld::shared()->railSystem());
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
			ImGui::TextWrapped(u8"左键点击已有轨道，把站点锚定到轨道上。");
			break;
		case RailPointAction::DeleteStation:
			ImGui::TextUnformatted(u8"删除站点");
			ImGui::TextWrapped(u8"左键点击站点删除。");
			break;
		case RailPointAction::AddRoutePoint:
			ImGui::TextUnformatted(u8"创建路点");
			ImGui::TextWrapped(u8"左键点击已有轨道，把路点锚定到轨道上。");
			break;
		case RailPointAction::DeleteRoutePoint:
			ImGui::TextUnformatted(u8"删除路点");
			ImGui::TextWrapped(u8"左键点击路点删除。");
			break;
		case RailPointAction::None:
		default:
			break;
		}
	}

	if (ImGui::Button(u8"退出编辑", ImVec2(-1.0f, 28.0f)))
	{
		state.clear();
	}
}

void RailEditorImGuiView::drawFloatingWindows(EditorState& state, RailEditingController& controller,
	InspectPanel& inspectPanel)
{
	if (m_lineWindowOpen)
	{
		ImGui::SetNextWindowSize(ImVec2(680.0f, 360.0f), ImGuiCond_FirstUseEver);
		if (ImGui::Begin(u8"线路面板", &m_lineWindowOpen, ImGuiWindowFlags_None))
		{
			drawRailLinePanel(state, controller);
		}
		ImGui::End();
	}
	RailSystem* railSystem = GameWorld::shared()->railSystem();
	if (railSystem)
	{
		railSystem->setLineOverviewVisible(m_lineWindowOpen);
	}
	if (m_trainWindowOpen)
	{
		ImGui::SetNextWindowSize(ImVec2(420.0f, 320.0f), ImGuiCond_FirstUseEver);
		if (ImGui::Begin(u8"车辆面板", &m_trainWindowOpen, ImGuiWindowFlags_None))
		{
			drawRailTrainPanel(controller, inspectPanel);
		}
		ImGui::End();
	}
}

void RailEditorImGuiView::drawRailTrackPanel(EditorState& state, RailEditingController& controller)
{
	float buttonWidth = 95.0f;
	drawRailToolButton(state, controller, u8"增加铁轨", trackState(RailTrackAction::AddTrack), buttonWidth);
	ImGui::SameLine();
	drawRailToolButton(state, controller, u8"删除铁轨", trackState(RailTrackAction::DeleteTrack), buttonWidth);
}

void RailEditorImGuiView::drawRailPointPanel(EditorState& state, RailEditingController& controller)
{
	float buttonWidth = 95.0f;
	drawRailToolButton(state, controller, u8"创建站点", pointState(RailPointAction::AddStation), buttonWidth);
	ImGui::SameLine();
	drawRailToolButton(state, controller, u8"删除站点", pointState(RailPointAction::DeleteStation), buttonWidth);
	drawRailToolButton(state, controller, u8"创建路点", pointState(RailPointAction::AddRoutePoint), buttonWidth);
	ImGui::SameLine();
	drawRailToolButton(state, controller, u8"删除路点", pointState(RailPointAction::DeleteRoutePoint), buttonWidth);
}

void RailEditorImGuiView::drawRailManagementPanel(InspectPanel& inspectPanel)
{
	if (ImGui::Button(u8"线路面板", ImVec2(120.0f, 30.0f)))
	{
		m_lineWindowOpen = true;
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"车辆面板", ImVec2(120.0f, 30.0f)))
	{
		m_trainWindowOpen = true;
	}
	if (ImGui::Button("Inspect", ImVec2(120.0f, 30.0f)))
	{
		inspectPanel.open();
	}
}

void RailEditorImGuiView::drawRailLinePanel(EditorState& state, RailEditingController& controller)
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
		controller.createLine();
	}
	ImGui::SameLine();
	if (controller.isLineControlEditState(state))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.45f);
	}
	if (ImGui::Button(u8"删除线路") && !controller.isLineControlEditState(state))
	{
		controller.deleteSelectedLine();
	}
	if (controller.isLineControlEditState(state))
	{
		ImGui::PopStyleVar();
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"取消预览"))
	{
		controller.cancelLinePreview(state);
	}

	ImGui::Separator();
	ImGui::Columns(2, "RailLineColumns", true);
	ImGui::SetColumnWidth(0, 245.0f);
	ImGui::TextUnformatted(u8"线路");
	ImGui::Separator();
	for (const RailLine& line : lineManager.lines())
	{
		const bool isSelected = line.id == lineManager.selectedLineId();
		ImGui::PushID(line.id);
		ImGui::ColorButton("lineColor", imguiColor(railLineColor(line.id)),
			ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2(14.0f, 14.0f));
		ImGui::SameLine();
		if (!line.isUsable)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.25f, 0.25f, 1.0f));
		}
		if (ImGui::Selectable((line.name + "##line").c_str(), isSelected, ImGuiSelectableFlags_None,
			ImVec2(150.0f, 0.0f)))
		{
			controller.selectLine(line.id);
		}
		if (!line.isUsable)
		{
			ImGui::PopStyleColor();
		}
		if (line.isUsable)
		{
			ImGui::TextDisabled(u8"可用 %.0fm  控制点 %d", line.totalLength,
				static_cast<int>(line.controlPoints.size()));
		}
		else
		{
			ImGui::TextDisabled(u8"不可用  控制点 %d", static_cast<int>(line.controlPoints.size()));
		}
		ImGui::PopID();
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
		ImGui::ColorButton("selectedLineColor", imguiColor(railLineColor(selectedLine->id)),
			ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2(16.0f, 16.0f));
		ImGui::SameLine();
		ImGui::Text("%s", selectedLine->name.c_str());
		ImGui::SameLine();
		if (ImGui::SmallButton(u8"编辑##lineEdit"))
		{
			controller.setRailState(state, lineState(RailLineAction::AddControlPoint));
			controller.selectLine(selectedLine->id);
		}
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
			const RailLineControlPoint& controlPoint = selectedLine->controlPoints[i];
			std::string label = controlPointLabel(railSystem, controlPoint);
			if (controlPoint.isResolved)
			{
				ImGui::Text("%d. %s  %.1fm", i + 1, label.c_str(), controlPoint.distanceOnLine);
			}
			else
			{
				ImGui::Text("%d. %s", i + 1, label.c_str());
			}
		}

		ImGui::Separator();
		ImGui::TextUnformatted(u8"添加控制点");
		for (const RailStation& station : railSystem->stationManager().stations())
		{
			std::string buttonId = station.name + "##addStationToLine" + std::to_string(station.id);
			if (ImGui::SmallButton(buttonId.c_str()))
			{
				controller.addStationToSelectedLine(station.id);
			}
			ImGui::SameLine();
		}
		for (const RailRoutePoint& routePoint : railSystem->routePointManager().routePoints())
		{
			std::string buttonId = routePoint.name + "##addRoutePointToLine" + std::to_string(routePoint.id);
			if (ImGui::SmallButton(buttonId.c_str()))
			{
				controller.addRoutePointToSelectedLine(routePoint.id);
			}
			ImGui::SameLine();
		}
	}
	ImGui::Columns(1, "RailLineColumns", false);
}

void RailEditorImGuiView::drawRailTrainPanel(RailEditingController& controller, InspectPanel& inspectPanel)
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
			controller.createTrain(m_newTrainCarriageCount);
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
					controller.assignTrainLine(train.id, line.id);
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
		controller.deleteTrain(trainToDelete);
	}
	if (trainToToggle != InvalidRailTrainId)
	{
		controller.toggleTrainRunning(trainToToggle);
	}
}

void RailEditorImGuiView::drawLineEditParameterPanel(EditorState& state, RailEditingController& controller,
	RailSystem* railSystem)
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
		controller.setRailState(state, lineState(RailLineAction::AddControlPoint));
	}
	ImGui::SameLine();
	if (ImGui::RadioButton(u8"删除控制点", state.rail.lineAction == RailLineAction::RemoveControlPoint))
	{
		controller.setRailState(state, lineState(RailLineAction::RemoveControlPoint));
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
			controller.removeControlPointFromSelectedLine(i);
			break;
		}
	}
}

void RailEditorImGuiView::drawRailToolButton(EditorState& state, RailEditingController& controller,
	const char* label, const RailEditorState& nextState, float width)
{
	bool isSelected = controller.isRailStateSelected(state, nextState);
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
			controller.setRailState(state, nextState);
		}
	}

	ImGui::PopStyleColor(3);
}

} // namespace tzw
