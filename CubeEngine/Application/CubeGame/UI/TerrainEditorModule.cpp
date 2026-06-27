#include "TerrainEditorModule.h"

#include "2D/IMGUISystem.h"
#include "CubeGame/BuildingSystem.h"
#include "CubeGame/GameWorld.h"

namespace tzw {

void TerrainEditorModule::drawMainTab(EditorModalState& state)
{
	ImGui::Text("Terrain Tool");
	ImGui::Separator();

	float buttonWidth = 95.0f;
	drawToolButton(state, "Carve Sphere", TerrainToolAction::CarveSphere, buttonWidth);
	ImGui::SameLine();
	drawToolButton(state, "Raise Sphere", TerrainToolAction::RaiseSphere, buttonWidth);
	ImGui::SameLine();
	drawToolButton(state, "Paint Sphere", TerrainToolAction::PaintSphere, buttonWidth);

	drawToolButton(state, "Carve Box", TerrainToolAction::CarveBox, buttonWidth);
	ImGui::SameLine();
	drawToolButton(state, "Raise Box", TerrainToolAction::RaiseBox, buttonWidth);
}

void TerrainEditorModule::drawParameterPanel(EditorModalState& state)
{
	if (!state.isTerrainActive())
	{
		ImGui::TextUnformatted(u8"未选择工具");
		return;
	}

	switch (state.terrainAction)
	{
	case TerrainToolAction::CarveSphere:
	case TerrainToolAction::RaiseSphere:
		ImGui::SliderFloat("Radius", &m_radius, 0.1f, 10.0f, "%.1f");
		ImGui::SliderFloat("Strength", &m_strength, 0.0f, 1.0f, "%.2f");
		break;
	case TerrainToolAction::CarveBox:
	case TerrainToolAction::RaiseBox:
		ImGui::SliderFloat("Strength", &m_strength, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Half X", &m_boxHalfX, 0.1f, 10.0f, "%.1f");
		ImGui::SliderFloat("Half Y", &m_boxHalfY, 0.1f, 10.0f, "%.1f");
		ImGui::SliderFloat("Half Z", &m_boxHalfZ, 0.1f, 10.0f, "%.1f");
		break;
	case TerrainToolAction::PaintSphere:
		ImGui::SliderFloat("Radius", &m_radius, 0.1f, 10.0f, "%.1f");
		ImGui::SliderInt("Material", &m_materialIndex, 0, 255);
		break;
	case TerrainToolAction::None:
	default:
		break;
	}

	if (ImGui::Button(u8"退出编辑", ImVec2(-1.0f, 28.0f)))
	{
		state.clear();
	}
}

bool TerrainEditorModule::handlePrimaryClick(const EditorModalState& state)
{
	if (!state.isTerrainActive())
	{
		return false;
	}

	applyTerrainEdit(state.terrainAction);
	return true;
}

void TerrainEditorModule::drawToolButton(EditorModalState& state, const char* label, TerrainToolAction action, float width)
{
	bool isSelected = isActionSelected(state, action);
	if (isSelected)
	{
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.5f, 0.8f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.3f, 0.6f, 0.9f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.2f, 0.5f, 0.8f, 1.0f));
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
	}

	if (ImGui::Selectable(label, isSelected, ImGuiSelectableFlags_None, ImVec2(width, 32.0f)))
	{
		if (isSelected)
		{
			state.clear();
		}
		else
		{
			setAction(state, action);
		}
	}

	ImGui::PopStyleColor(3);
}

void TerrainEditorModule::setAction(EditorModalState& state, TerrainToolAction action)
{
	state.clear();
	state.module = EditorModuleId::Terrain;
	state.terrainAction = action;
}

void TerrainEditorModule::applyTerrainEdit(TerrainToolAction action)
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

bool TerrainEditorModule::isActionSelected(const EditorModalState& state, TerrainToolAction action) const
{
	return state.module == EditorModuleId::Terrain && state.terrainAction == action;
}

} // namespace tzw
