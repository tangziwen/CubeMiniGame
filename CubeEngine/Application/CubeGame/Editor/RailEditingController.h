#ifndef TZW_RAIL_EDITING_CONTROLLER_H
#define TZW_RAIL_EDITING_CONTROLLER_H

#include "CubeGame/Editor/EditorState.h"
#include "CubeGame/Rail/RailLine.h"
#include "CubeGame/Rail/RailTrain.h"

namespace tzw {

class InspectPanel;
class RailSystem;

class RailEditingController
{
public:
	void setRailState(EditorState& state, const RailEditorState& railState);
	bool isLineControlEditState(const EditorState& state) const;
	bool isRailStateSelected(const EditorState& state, const RailEditorState& railState) const;

	bool handlePrimaryClick(const EditorState& state, InspectPanel& inspectPanel);
	bool handleSecondaryClick(const EditorState& state);
	void syncWorldVisuals(const EditorState& state);
	void hideWorldVisuals();

	void createLine();
	void deleteSelectedLine();
	void cancelLinePreview(EditorState& state);
	void selectLine(RailLineId lineId);
	void addStationToSelectedLine(RailStationId stationId);
	void addRoutePointToSelectedLine(RailRoutePointId routePointId);
	void removeControlPointFromSelectedLine(int controlPointIndex);

	void createTrain(int carriageCount);
	void deleteTrain(RailTrainId trainId);
	void toggleTrainRunning(RailTrainId trainId);
	void assignTrainLine(RailTrainId trainId, RailLineId lineId);

private:
	bool handleInspectPrimaryClick(RailSystem* railSystem, InspectPanel& inspectPanel);

	RailEditorState m_syncedRailState;
};

} // namespace tzw

#endif // TZW_RAIL_EDITING_CONTROLLER_H
