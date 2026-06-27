#include "InspectPanel.h"

#include "2D/IMGUISystem.h"
#include "2D/NotificationSystem.h"
#include "CubeGame/GameWorld.h"
#include "CubeGame/Rail/RailSystem.h"

#include <cstdio>

namespace tzw {

namespace
{
const vec4 InspectOutlineColor(1.0f, 0.82f, 0.16f, 1.0f);

const char* placementModeText(RailTrainPlacementMode mode)
{
	switch (mode)
	{
	case RailTrainPlacementMode::OnLine:
		return u8"线路上";
	case RailTrainPlacementMode::Detached:
		return u8"脱离线路";
	case RailTrainPlacementMode::Unplaced:
	default:
		return u8"未放置";
	}
}

void pushInvalidNameNotification()
{
	NotificationSystem::shared()->push("name cannot be empty", NotificationLevel::Warning);
}
}

void InspectPanel::draw(RailSystem* railSystem)
{
	if (!m_isOpen)
	{
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(320.0f, 260.0f), ImGuiCond_FirstUseEver);
	if (ImGui::Begin(u8"Inspect", &m_isOpen, ImGuiWindowFlags_None))
	{
		if (!railSystem || !m_target.isValid())
		{
			drawEmpty();
		}
		else if (!targetExists(railSystem))
		{
			clearTargetOutline(railSystem);
			m_target = RailInspectTarget();
			drawEmpty();
		}
		else
		{
			switch (m_target.kind)
			{
			case RailInspectTargetKind::Train:
				drawTrain(railSystem);
				break;
			case RailInspectTargetKind::Station:
				drawStation(railSystem);
				break;
			case RailInspectTargetKind::RoutePoint:
				drawRoutePoint(railSystem);
				break;
			case RailInspectTargetKind::None:
			default:
				drawEmpty();
				break;
			}
		}
	}
	ImGui::End();
	if (!m_isOpen)
	{
		clearTargetOutline(railSystem);
		m_target = RailInspectTarget();
	}
}

void InspectPanel::open()
{
	m_isOpen = true;
}

void InspectPanel::close()
{
	m_isOpen = false;
	clearTargetOutline(GameWorld::shared()->railSystem());
	m_target = RailInspectTarget();
}

bool InspectPanel::isOpen() const
{
	return m_isOpen;
}

const RailInspectTarget& InspectPanel::target() const
{
	return m_target;
}

bool InspectPanel::isInspecting(const RailInspectTarget& target) const
{
	return m_target.isValid() && m_target.equals(target);
}

void InspectPanel::clearTarget(RailSystem* railSystem)
{
	clearTargetOutline(railSystem);
	m_target = RailInspectTarget();
	m_nameBuffer = {};
}

void InspectPanel::inspect(const RailInspectTarget& target, RailSystem* railSystem)
{
	clearTargetOutline(railSystem);
	m_target = target;
	applyTargetOutline(railSystem, m_target, true);
	m_outlinedTarget = m_target;
	if (railSystem && target.kind == RailInspectTargetKind::Station)
	{
		const RailStation* station = railSystem->stationManager().station(target.stationId);
		if (station)
		{
			syncNameBuffer(station->name);
		}
	}
	else if (railSystem && target.kind == RailInspectTargetKind::RoutePoint)
	{
		const RailRoutePoint* routePoint = railSystem->routePointManager().routePoint(target.routePointId);
		if (routePoint)
		{
			syncNameBuffer(routePoint->name);
		}
	}
	open();
}

void InspectPanel::drawEmpty()
{
	ImGui::TextUnformatted(u8"未选择对象");
}

void InspectPanel::drawTrain(RailSystem* railSystem)
{
	const RailTrain* train = railSystem->trainManager().train(m_target.trainId);
	if (!train)
	{
		drawEmpty();
		return;
	}

	const RailLine* line = railSystem->lineManager().line(train->lineId);
	ImGui::Text("%s", train->name.c_str());
	ImGui::Separator();
	ImGui::Text(u8"线路: %s", line ? line->name.c_str() : u8"未分配");
	ImGui::Text(u8"位置: %s", placementModeText(train->placementMode));
	ImGui::Text(u8"状态: %s", train->isManuallyStopped ? u8"手动停车" : (train->active ? u8"运行" : u8"停止"));
	if (train->isDwelling)
	{
		ImGui::Text(u8"到站停靠: %.1fs", train->dwellRemainingSeconds);
	}
	ImGui::Text(u8"速度: %.2f", train->speed);
	ImGui::Text(u8"车厢数量: %d", train->carriageCount);
	ImGui::Text(u8"Pose: %s", train->pose.valid ? u8"有效" : u8"无效");
}

void InspectPanel::drawStation(RailSystem* railSystem)
{
	const RailStation* station = railSystem->stationManager().station(m_target.stationId);
	if (!station)
	{
		drawEmpty();
		return;
	}

	if (std::string(m_nameBuffer.data()) != station->name && !ImGui::IsAnyItemActive())
	{
		syncNameBuffer(station->name);
	}

	ImGui::Text(u8"Station #%d", station->id);
	ImGui::Separator();
	ImGui::Text(u8"平台数量: %d", static_cast<int>(station->platforms.size()));
	const RailPlatform* platform = railSystem->stationManager().firstPlatform(station->id);
	ImGui::Text(u8"Anchor: %s", platform && platform->anchorId != InvalidRailAnchorId ? u8"有效" : u8"无效");

	if (ImGui::InputText(u8"名称", m_nameBuffer.data(), m_nameBuffer.size(), ImGuiInputTextFlags_EnterReturnsTrue)
		|| ImGui::IsItemDeactivatedAfterEdit())
	{
		commitStationName(railSystem);
	}
}

void InspectPanel::drawRoutePoint(RailSystem* railSystem)
{
	const RailRoutePoint* routePoint = railSystem->routePointManager().routePoint(m_target.routePointId);
	if (!routePoint)
	{
		drawEmpty();
		return;
	}

	if (std::string(m_nameBuffer.data()) != routePoint->name && !ImGui::IsAnyItemActive())
	{
		syncNameBuffer(routePoint->name);
	}

	ImGui::Text(u8"RoutePoint #%d", routePoint->id);
	ImGui::Separator();
	ImGui::Text(u8"Anchor: %s", routePoint->anchorId != InvalidRailAnchorId ? u8"有效" : u8"无效");

	if (ImGui::InputText(u8"名称", m_nameBuffer.data(), m_nameBuffer.size(), ImGuiInputTextFlags_EnterReturnsTrue)
		|| ImGui::IsItemDeactivatedAfterEdit())
	{
		commitRoutePointName(railSystem);
	}
}

void InspectPanel::applyTargetOutline(RailSystem* railSystem, const RailInspectTarget& target, bool enabled)
{
	if (!railSystem || !target.isValid())
	{
		return;
	}

	switch (target.kind)
	{
	case RailInspectTargetKind::Train:
		railSystem->setTrainOutline(target.trainId, enabled, InspectOutlineColor);
		break;
	case RailInspectTargetKind::Station:
		railSystem->setStationOutline(target.stationId, enabled, InspectOutlineColor);
		break;
	case RailInspectTargetKind::RoutePoint:
		railSystem->setRoutePointOutline(target.routePointId, enabled, InspectOutlineColor);
		break;
	case RailInspectTargetKind::None:
	default:
		break;
	}
}

void InspectPanel::clearTargetOutline(RailSystem* railSystem)
{
	if (!railSystem || !m_outlinedTarget.isValid())
	{
		return;
	}
	applyTargetOutline(railSystem, m_outlinedTarget, false);
	m_outlinedTarget = RailInspectTarget();
}

void InspectPanel::syncNameBuffer(const std::string& name)
{
	std::snprintf(m_nameBuffer.data(), m_nameBuffer.size(), "%s", name.c_str());
}

bool InspectPanel::commitStationName(RailSystem* railSystem)
{
	if (!railSystem)
	{
		return false;
	}
	const std::string name = m_nameBuffer.data();
	if (name.empty())
	{
		pushInvalidNameNotification();
		const RailStation* station = railSystem->stationManager().station(m_target.stationId);
		if (station)
		{
			syncNameBuffer(station->name);
		}
		return false;
	}
	return railSystem->renameStation(m_target.stationId, name);
}

bool InspectPanel::commitRoutePointName(RailSystem* railSystem)
{
	if (!railSystem)
	{
		return false;
	}
	const std::string name = m_nameBuffer.data();
	if (name.empty())
	{
		pushInvalidNameNotification();
		const RailRoutePoint* routePoint = railSystem->routePointManager().routePoint(m_target.routePointId);
		if (routePoint)
		{
			syncNameBuffer(routePoint->name);
		}
		return false;
	}
	return railSystem->renameRoutePoint(m_target.routePointId, name);
}

bool InspectPanel::targetExists(RailSystem* railSystem) const
{
	if (!railSystem)
	{
		return false;
	}

	switch (m_target.kind)
	{
	case RailInspectTargetKind::Train:
		return railSystem->trainManager().train(m_target.trainId) != nullptr;
	case RailInspectTargetKind::Station:
		return railSystem->stationManager().station(m_target.stationId) != nullptr;
	case RailInspectTargetKind::RoutePoint:
		return railSystem->routePointManager().routePoint(m_target.routePointId) != nullptr;
	case RailInspectTargetKind::None:
	default:
		return false;
	}
}

} // namespace tzw
