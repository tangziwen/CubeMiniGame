#include "TerrainEditingController.h"

#include "CubeGame/BuildingSystem.h"
#include "CubeGame/GameWorld.h"

namespace tzw {

void TerrainEditingController::selectAction(EditorState& state, TerrainToolAction action)
{
	state.clear();
	state.module = EditorModuleId::Terrain;
	state.terrainAction = action;
}

bool TerrainEditingController::handlePrimaryClick(const EditorState& state)
{
	if (!state.isTerrainActive())
	{
		return false;
	}

	applyTerrainEdit(state.terrainAction);
	return true;
}

bool TerrainEditingController::isActionSelected(const EditorState& state, TerrainToolAction action) const
{
	return state.module == EditorModuleId::Terrain && state.terrainAction == action;
}

float& TerrainEditingController::radius()
{
	return m_radius;
}

float& TerrainEditingController::strength()
{
	return m_strength;
}

float& TerrainEditingController::density()
{
	return m_density;
}

float& TerrainEditingController::boxHalfX()
{
	return m_boxHalfX;
}

float& TerrainEditingController::boxHalfY()
{
	return m_boxHalfY;
}

float& TerrainEditingController::boxHalfZ()
{
	return m_boxHalfZ;
}

int& TerrainEditingController::materialIndex()
{
	return m_materialIndex;
}

void TerrainEditingController::applyTerrainEdit(TerrainToolAction action)
{
	auto player = GameWorld::shared()->getPlayer();
	if (!player)
	{
		return;
	}

	switch (action)
	{
	case TerrainToolAction::CarveSphere:
		BuildingSystem::shared()->terrainCarveSphere(m_radius, m_strength, PlacementMode::CursorBased);
		break;
	case TerrainToolAction::RaiseSphere:
		BuildingSystem::shared()->terrainRaiseSphere(m_radius, m_strength, PlacementMode::CursorBased);
		break;
	case TerrainToolAction::CarveBox:
		BuildingSystem::shared()->terrainCarveBox(vec3(m_boxHalfX, m_boxHalfY, m_boxHalfZ), m_strength, PlacementMode::CursorBased);
		break;
	case TerrainToolAction::RaiseBox:
		BuildingSystem::shared()->terrainRaiseBox(vec3(m_boxHalfX, m_boxHalfY, m_boxHalfZ), m_strength, PlacementMode::CursorBased);
		break;
	case TerrainToolAction::PaintSphere:
		BuildingSystem::shared()->terrainPaint(m_materialIndex, m_radius, PlacementMode::CursorBased);
		break;
	case TerrainToolAction::None:
	default:
		break;
	}
}

} // namespace tzw
