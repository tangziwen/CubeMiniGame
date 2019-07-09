package.path = Engine.shared():getFilePath("Script/?.lua;") .. package.path
g_include_module = {}
function include(moduleName)
	require(moduleName)
	g_include_module[moduleName] = 1
end

function tzw_engine_reload()
	print("on tzw_engine_reload")
	for k, v in pairs(g_include_module) do
		package.loaded[k] = nil
		require(k)
	end
end

include("Main")