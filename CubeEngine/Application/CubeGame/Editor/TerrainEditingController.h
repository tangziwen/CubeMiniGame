#ifndef TZW_TERRAIN_EDITING_CONTROLLER_H
#define TZW_TERRAIN_EDITING_CONTROLLER_H

#include "CubeGame/Editor/EditorState.h"

namespace tzw {

class TerrainEditingController
{
public:
	void selectAction(EditorState& state, TerrainToolAction action);
	bool handlePrimaryClick(const EditorState& state);
	bool isActionSelected(const EditorState& state, TerrainToolAction action) const;

	float& radius();
	float& strength();
	float& density();
	float& boxHalfX();
	float& boxHalfY();
	float& boxHalfZ();
	int& materialIndex();

private:
	void applyTerrainEdit(TerrainToolAction action);

	float m_radius = 2.0f;
	float m_strength = 0.5f;
	float m_density = 0.5f;
	float m_boxHalfX = 2.0f;
	float m_boxHalfY = 2.0f;
	float m_boxHalfZ = 2.0f;
	int m_materialIndex = 0;
};

} // namespace tzw

#endif // TZW_TERRAIN_EDITING_CONTROLLER_H
