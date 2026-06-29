#ifndef TZW_RAIL_EDITOR_IMGUI_VIEW_H
#define TZW_RAIL_EDITOR_IMGUI_VIEW_H

#include "CubeGame/Editor/EditorState.h"

namespace tzw {

class InspectPanel;
class RailEditingController;
class RailSystem;

class RailEditorImGuiView
{
public:
	void drawMainTab(EditorState& state, RailEditingController& controller, InspectPanel& inspectPanel);
	void drawParameterPanel(EditorState& state, RailEditingController& controller);
	void drawFloatingWindows(EditorState& state, RailEditingController& controller, InspectPanel& inspectPanel);

private:
	void drawRailTrackPanel(EditorState& state, RailEditingController& controller);
	void drawRailPointPanel(EditorState& state, RailEditingController& controller);
	void drawRailManagementPanel(InspectPanel& inspectPanel);
	void drawRailLinePanel(EditorState& state, RailEditingController& controller);
	void drawRailTrainPanel(RailEditingController& controller, InspectPanel& inspectPanel);
	void drawLineEditParameterPanel(EditorState& state, RailEditingController& controller, RailSystem* railSystem);
	void drawRailToolButton(EditorState& state, RailEditingController& controller,
		const char* label, const RailEditorState& nextState, float width);

	bool m_lineWindowOpen = false;
	bool m_trainWindowOpen = false;
	int m_newTrainCarriageCount = 0;
};

} // namespace tzw

#endif // TZW_RAIL_EDITOR_IMGUI_VIEW_H
