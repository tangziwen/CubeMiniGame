#ifndef TZW_EDITOR_CONTROLLER_H
#define TZW_EDITOR_CONTROLLER_H

#include "CubeGame/Editor/EditorState.h"
#include "CubeGame/Editor/RailEditingController.h"
#include "CubeGame/Editor/TerrainEditingController.h"

namespace tzw {

class InspectPanel;

class EditorController
{
public:
	EditorState& state();
	const EditorState& state() const;
	TerrainEditingController& terrain();
	RailEditingController& rail();

	void switchToTab(int tabIndex, EditorModuleId moduleId);
	void onFrameUpdate(float delta);
	bool onMousePress(int button, InspectPanel& inspectPanel);

private:
	EditorState m_state;
	TerrainEditingController m_terrainController;
	RailEditingController m_railController;
	int m_activeTab = 0;
};

} // namespace tzw

#endif // TZW_EDITOR_CONTROLLER_H
