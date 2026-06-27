#ifndef TZW_RAIL_EDITOR_MODULE_H
#define TZW_RAIL_EDITOR_MODULE_H

#include "CubeGame/UI/EditorModalState.h"

namespace tzw {

class InspectPanel;
class RailSystem;

class RailEditorModule
{
public:
	void drawMainTab(EditorModalState& state, InspectPanel& inspectPanel);
	void drawParameterPanel(EditorModalState& state);
	void drawFloatingWindows(EditorModalState& state, InspectPanel& inspectPanel);
	bool handlePrimaryClick(const EditorModalState& state, InspectPanel& inspectPanel);
	bool handleSecondaryClick(const EditorModalState& state);
	void syncWorldVisuals(const EditorModalState& state);
	void hideWorldVisuals();

private:
	void drawRailTrackPanel(EditorModalState& state);
	void drawRailLinePanel(EditorModalState& state);
	void drawRailTrainPanel(InspectPanel& inspectPanel);
	void drawLineEditParameterPanel(EditorModalState& state, RailSystem* railSystem);
	void drawRailToolButton(EditorModalState& state, const char* label, const RailEditorState& nextState, float width);
	void setRailState(EditorModalState& state, const RailEditorState& railState);
	bool handleInspectPrimaryClick(RailSystem* railSystem, InspectPanel& inspectPanel);
	bool isLineControlEditState(const EditorModalState& state) const;
	bool isRailStateSelected(const EditorModalState& state, const RailEditorState& railState) const;

	RailEditorState m_syncedRailState;
	bool m_lineWindowOpen = false;
	bool m_trainWindowOpen = false;
	int m_newTrainCarriageCount = 0;
};

} // namespace tzw

#endif // TZW_RAIL_EDITOR_MODULE_H
