#ifndef TZW_TERRAIN_EDITOR_MODULE_H
#define TZW_TERRAIN_EDITOR_MODULE_H

#include "CubeGame/UI/EditorModalState.h"

namespace tzw {

class TerrainEditorModule
{
public:
	void drawMainTab(EditorModalState& state);
	void drawParameterPanel(EditorModalState& state);
	bool handlePrimaryClick(const EditorModalState& state);

private:
	void drawToolButton(EditorModalState& state, const char* label, TerrainToolAction action, float width);
	void setAction(EditorModalState& state, TerrainToolAction action);
	void applyTerrainEdit(TerrainToolAction action);
	bool isActionSelected(const EditorModalState& state, TerrainToolAction action) const;

	float m_radius = 2.0f;
	float m_strength = 0.5f;
	float m_density = 0.5f;
	float m_boxHalfX = 2.0f;
	float m_boxHalfY = 2.0f;
	float m_boxHalfZ = 2.0f;
	int m_materialIndex = 0;
};

} // namespace tzw

#endif // TZW_TERRAIN_EDITOR_MODULE_H
