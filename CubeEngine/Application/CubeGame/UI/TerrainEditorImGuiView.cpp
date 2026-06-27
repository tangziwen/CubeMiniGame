#include "TerrainEditorImGuiView.h"

#include "2D/IMGUISystem.h"
#include "CubeGame/Editor/TerrainEditingController.h"

namespace tzw {

void TerrainEditorImGuiView::drawMainTab(EditorState& state, TerrainEditingController& controller)
{
	ImGui::Text("Terrain Tool");
	ImGui::Separator();

	float buttonWidth = 95.0f;
	drawToolButton(state, controller, "Carve Sphere", TerrainToolAction::CarveSphere, buttonWidth);
	ImGui::SameLine();
	drawToolButton(state, controller, "Raise Sphere", TerrainToolAction::RaiseSphere, buttonWidth);
	ImGui::SameLine();
	drawToolButton(state, controller, "Paint Sphere", TerrainToolAction::PaintSphere, buttonWidth);

	drawToolButton(state, controller, "Carve Box", TerrainToolAction::CarveBox, buttonWidth);
	ImGui::SameLine();
	drawToolButton(state, controller, "Raise Box", TerrainToolAction::RaiseBox, buttonWidth);
}

void TerrainEditorImGuiView::drawParameterPanel(EditorState& state, TerrainEditingController& controller)
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
		ImGui::SliderFloat("Radius", &controller.radius(), 0.1f, 10.0f, "%.1f");
		ImGui::SliderFloat("Strength", &controller.strength(), 0.0f, 1.0f, "%.2f");
		break;
	case TerrainToolAction::CarveBox:
	case TerrainToolAction::RaiseBox:
		ImGui::SliderFloat("Strength", &controller.strength(), 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Half X", &controller.boxHalfX(), 0.1f, 10.0f, "%.1f");
		ImGui::SliderFloat("Half Y", &controller.boxHalfY(), 0.1f, 10.0f, "%.1f");
		ImGui::SliderFloat("Half Z", &controller.boxHalfZ(), 0.1f, 10.0f, "%.1f");
		break;
	case TerrainToolAction::PaintSphere:
		ImGui::SliderFloat("Radius", &controller.radius(), 0.1f, 10.0f, "%.1f");
		ImGui::SliderInt("Material", &controller.materialIndex(), 0, 255);
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

void TerrainEditorImGuiView::drawToolButton(EditorState& state, TerrainEditingController& controller,
	const char* label, TerrainToolAction action, float width)
{
	bool isSelected = controller.isActionSelected(state, action);
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
			controller.selectAction(state, action);
		}
	}

	ImGui::PopStyleColor(3);
}

} // namespace tzw
