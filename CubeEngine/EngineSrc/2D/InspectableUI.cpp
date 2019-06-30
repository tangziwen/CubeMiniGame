#include "InspectableUI.h"
#include "2D/GUISystem.h"
namespace tzw
{
	std::string InspectableUI::getTitle()
	{

	return "title";
	}

	void InspectableUI::inspect()
	{
	
	}

	void InspectableUI::bindFloat(std::string name, float * value, float min, float max, const char * fmt /*= "%.2f"*/)
	{
		ImGui::SliderFloat(name.c_str(), value, min, max, fmt);
	}

	void InspectableUI::bindColor(std::string name, vec3 * colorIn)
	{
		static ImVec4 color = ImVec4(colorIn->x, colorIn->y, colorIn->z, 1.0);
		static bool alpha_preview = false;
		static bool alpha_half_preview = false;
		static bool drag_and_drop = true;
		static bool options_menu = true;
		static bool hdr = false;
		int misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : 1<<9) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
		ImGui::ColorEdit3(name.c_str(), (float*)&color, misc_flags);
		colorIn->x = color.x;
		colorIn->y = color.y;
		colorIn->z = color.z;
	}

}