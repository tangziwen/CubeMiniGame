#ifndef TZW_TERRAIN_EDITOR_IMGUI_VIEW_H
#define TZW_TERRAIN_EDITOR_IMGUI_VIEW_H

#include "CubeGame/Editor/EditorState.h"

namespace tzw {

class TerrainEditingController;

class TerrainEditorImGuiView
{
public:
	void drawMainTab(EditorState& state, TerrainEditingController& controller);
	void drawParameterPanel(EditorState& state, TerrainEditingController& controller);

private:
	void drawToolButton(EditorState& state, TerrainEditingController& controller,
		const char* label, TerrainToolAction action, float width);
};

} // namespace tzw

#endif // TZW_TERRAIN_EDITOR_IMGUI_VIEW_H
