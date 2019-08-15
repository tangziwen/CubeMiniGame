UiUtil = {}
UiUtil.isPop = true

UiUtil.infoStr = ""
function UiUtil.handlePopText()
	if UiUtil.isPop then
		ImGui.OpenPopup("about");
		UiUtil.isPop = false
	end
	-- ImGui.SetNextWindowSize(ImGui.ImVec2(200, 300), 0)
	if (ImGui.BeginPopupModal("about", ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) then
		ImGui.TextWrapped(UiUtil.infoStr)
		if ImGui.Button("OK", ImGui.ImVec2(160, 35)) then
			ImGui.Button("End Button", ImGui.ImVec2(160, 35))
			ImGui.CloseCurrentPopup()
		end
		ImGui.EndPopup()
	end
end

function UiUtil.popText(infoStr)
	UiUtil.isPop = true
	UiUtil.infoStr = infoStr
end

return UiUtil