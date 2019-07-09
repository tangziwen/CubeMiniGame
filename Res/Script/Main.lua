include("KeyConfig")
Main = {}
--init
function tzw_engine_init()
	print("lua init")
end

local isOpenTestWindow = true
--ui update
function tzw_engine_ui_update(dt)
	--print ("engine update", dt)
	if isOpenTestWindow
	then
		isOpenTestWindow = ImGui.Begin("caonima", 0, isOpenTestWindow)
		ImGui.SmallButton("11111")
		ImGui.Text("cao cao ")
		ImGui.Text("cao cao ")
		ImGui.Text("cao cao ")
		ImGui.Text("cao cao ")
		btnSize = ImGui.ImVec2(50, 50)
		ImGui.ColorButton("##preview", ImGui.ImVec4(1.0, 0, 0, 0), 0, btnSize)
		ImGui.End()
	end
end

--input event
function tzw_engine_input_event(input_event)
	print (input_event.keycode == TZW_KEY_D)
end

return Main