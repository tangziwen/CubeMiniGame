#include "RailEditingController.h"

#include "2D/NotificationSystem.h"
#include "CubeGame/BuildingSystem.h"
#include "CubeGame/GameWorld.h"
#include "CubeGame/Rail/RailSystem.h"
#include "CubeGame/UI/InspectPanel.h"

namespace tzw {

namespace
{
void notifyRailEditResult(RailEditResult result)
{
	if (result != RailEditResult::Success)
	{
		NotificationSystem::shared()->push("invalid operation", NotificationLevel::Warning);
	}
}
}

void RailEditingController::setRailState(EditorState& state, const RailEditorState& railState)
{
	state.clear();
	state.module = EditorModuleId::Rail;
	state.rail = railState;
}

bool RailEditingController::isLineControlEditState(const EditorState& state) const
{
	return state.module == EditorModuleId::Rail && state.rail.isLineControlEditState();
}

bool RailEditingController::isRailStateSelected(const EditorState& state, const RailEditorState& railState) const
{
	return state.module == EditorModuleId::Rail && state.rail == railState;
}

bool RailEditingController::handlePrimaryClick(const EditorState& state, InspectPanel& inspectPanel)
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

bool RailEditingController::handleSecondaryClick(const EditorState& state)
{
	RailSystem* railSystem = GameWorld::shared()->railSystem();
	if (railSystem && state.module == EditorModuleId::Rail && state.rail.isTrackEditState())
	{
		railSystem->cancelTrackEdit();
		return true;
	}
	return false;
}

void RailEditingController::syncWorldVisuals(const EditorState& state)
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

void RailEditingController::hideWorldVisuals()
{
	RailSystem* railSystem = GameWorld::shared()->railSystem();
	if (railSystem)
	{
		railSystem->setLineOverviewVisible(false);
		railSystem->hideAllEditorVisuals();
	}
	m_syncedRailState = RailEditorState();
}

void RailEditingController::createLine()
{
	RailSystem* railSystem = GameWorld::shared()->railSystem();
	if (!railSystem)
	{
		return;
	}
	RailLineManager& lineManager = railSystem->lineManager();
	const RailLineId lineId = lineManager.createLine();
	lineManager.setSelectedLineId(lineId);
	railSystem->setLinePreview(lineId);
}

void RailEditingController::deleteSelectedLine()
{
	RailSystem* railSystem = GameWorld::shared()->railSystem();
	if (!railSystem)
	{
		return;
	}

	RailLineManager& lineManager = railSystem->lineManager();
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

void RailEditingController::cancelLinePreview(EditorState& state)
{
	RailSystem* railSystem = GameWorld::shared()->railSystem();
	if (!railSystem)
	{
		return;
	}
	railSystem->lineManager().setSelectedLineId(InvalidRailLineId);
	railSystem->setLineOverviewVisible(false);
	if (isLineControlEditState(state))
	{
		state.clear();
	}
	railSystem->clearLinePreview();
}

void RailEditingController::selectLine(RailLineId lineId)
{
	RailSystem* railSystem = GameWorld::shared()->railSystem();
	if (!railSystem)
	{
		return;
	}
	railSystem->lineManager().setSelectedLineId(lineId);
	railSystem->setLinePreview(lineId);
}

void RailEditingController::addStationToSelectedLine(RailStationId stationId)
{
	RailSystem* railSystem = GameWorld::shared()->railSystem();
	if (!railSystem)
	{
		return;
	}
	railSystem->addStationToSelectedLine(stationId);
	const RailLineId lineId = railSystem->lineManager().selectedLineId();
	if (lineId != InvalidRailLineId)
	{
		railSystem->setLinePreview(lineId);
	}
}

void RailEditingController::addRoutePointToSelectedLine(RailRoutePointId routePointId)
{
	RailSystem* railSystem = GameWorld::shared()->railSystem();
	if (!railSystem)
	{
		return;
	}
	railSystem->addRoutePointToSelectedLine(routePointId);
	const RailLineId lineId = railSystem->lineManager().selectedLineId();
	if (lineId != InvalidRailLineId)
	{
		railSystem->setLinePreview(lineId);
	}
}

void RailEditingController::removeControlPointFromSelectedLine(int controlPointIndex)
{
	RailSystem* railSystem = GameWorld::shared()->railSystem();
	if (!railSystem)
	{
		return;
	}
	RailLineManager& lineManager = railSystem->lineManager();
	const RailLineId lineId = lineManager.selectedLineId();
	if (lineId == InvalidRailLineId)
	{
		return;
	}
	if (lineManager.removeControlPointAt(lineId, controlPointIndex, railSystem->network(),
		railSystem->anchorManager(), railSystem->stationManager(), railSystem->routePointManager()))
	{
		railSystem->setLinePreview(lineId);
		railSystem->trainManager().refreshAfterLineRebuild(lineManager);
	}
}

void RailEditingController::createTrain(int carriageCount)
{
	RailSystem* railSystem = GameWorld::shared()->railSystem();
	if (railSystem)
	{
		railSystem->trainManager().createTrain(carriageCount);
	}
}

void RailEditingController::deleteTrain(RailTrainId trainId)
{
	RailSystem* railSystem = GameWorld::shared()->railSystem();
	if (railSystem)
	{
		railSystem->trainManager().deleteTrain(trainId);
	}
}

void RailEditingController::toggleTrainRunning(RailTrainId trainId)
{
	RailSystem* railSystem = GameWorld::shared()->railSystem();
	if (railSystem)
	{
		railSystem->trainManager().toggleTrainRunning(trainId);
	}
}

void RailEditingController::assignTrainLine(RailTrainId trainId, RailLineId lineId)
{
	RailSystem* railSystem = GameWorld::shared()->railSystem();
	if (railSystem)
	{
		railSystem->trainManager().assignLine(trainId, lineId, railSystem->lineManager());
	}
}

bool RailEditingController::handleInspectPrimaryClick(RailSystem* railSystem, InspectPanel& inspectPanel)
{
	if (!railSystem)
	{
		return false;
	}

	RailInspectTarget target;
	if (!railSystem->pickInspectTarget(PlacementMode::CursorBased, target))
	{
		inspectPanel.clearTarget(railSystem);
		return true;
	}

	if (inspectPanel.isInspecting(target))
	{
		inspectPanel.clearTarget(railSystem);
		return true;
	}

	inspectPanel.inspect(target, railSystem);
	return true;
}

} // namespace tzw
