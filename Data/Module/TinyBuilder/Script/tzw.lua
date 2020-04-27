--add the search result
package.path = "Data/module/TinyBuilder/Script/?.lua;" .. package.path
hotfix = require("hotfix.hotfix")
g_include_module = {}
function include(moduleName)
	local r = require(moduleName)
	g_include_module[moduleName] = 1
	return r
end

function tzw_engine_reload()
	print("on tzw_engine_reload")
	for k, v in pairs(g_include_module) do
		-- package.loaded[k] = nil
		-- require(k)
		hotfix.hotfix_module(k)
		print("hot fix for"..k)
	end
end


function test_fuck(a,b,c,d,e)
	print("In Lua Print!!!!!!!",a,b,c,d,e)
end

Main = include("Main")
-- init
function tzw_engine_init()
	Main.onEngineInit()
end

-- input event
function tzw_engine_input_event(input_event)
	Main.onEngineInputEvent(input_event)
end

--ui update
function tzw_engine_ui_update(dt)
	Main.handleUIUpdate(dt)
end

--specified window draw
function tzw_game_draw_window(arg)
	return Main.onDrawWindow(arg)
end