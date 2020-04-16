--[[
Lua 5.2/5.3 hotfix. Hot update functions and keep old data.
Author: Jin Qing ( http://blog.csdn.net/jq0123 )
--]]

local M = {}

local module_updater = require("hotfix.internal.module_updater")
local functions_replacer = require("hotfix.internal.functions_replacer")

-- Do not update and replace protected objects.
local protected = {}

-- To protect self.
local function add_self_to_protect()
    M.add_protect{
        M,
        M.hotfix_module,
        M.log_error,
        M.log_info,
        M.log_debug,
        M.add_protect,
        M.remove_protect,
        module_updater,
        module_updater.log_debug,
        module_updater.update_loaded_module,
        functions_replacer,
        functions_replacer.replace_all,
    }
end  -- add_self_to_protect

-- Hotfix module with new module object.
-- Update package.loaded[module_name] and replace all functions.
-- module_obj is the newly loaded module object.
local function hotfix_module_with_obj(module_name, module_obj)
    assert("string" == type(module_name))
    add_self_to_protect()
    module_updater.log_debug = M.log_debug

    -- Step 1: Update package.loaded[module_name], recording updated functions.
    local updated_function_map = module_updater.update_loaded_module(
        module_name, protected, module_obj)
    -- Step 2: Replace old functions with new ones in module_obj, _G and registry.
    functions_replacer.replace_all(protected, updated_function_map, module_obj)
end  -- hotfix_module_with_obj()

-- Hotfix module.
-- Skip unloaded module.
-- Usage: hotfix_module("mymodule.sub_module")
-- Returns package.loaded[module_name].
function M.hotfix_module(module_name)
    assert("string" == type(module_name))
    if not package.loaded[module_name] then
        M.log_debug("Skip unloaded module: " .. module_name)
        return package.loaded[module_name]
    end
    M.log_debug("Hot fix module: " .. module_name)

    local file_path = assert(package.searchpath(module_name, package.path))
    local fp = assert(io.open(file_path))
    local chunk = fp:read("*all")
    fp:close()

    -- Load chunk.
    local func = assert(load(chunk, '@'..file_path))
    local ok, obj = assert(pcall(func))
    if nil == obj then obj = true end  -- obj may be false

    hotfix_module_with_obj(module_name, obj)
    return package.loaded[module_name]
end

-- User can set log functions. Default is no log.
-- Like: require("hotfix").log_info = function(s) mylog:info(s) end
function M.log_error(msg_str) end
function M.log_info(msg_str) end
function M.log_debug(msg_str) end

-- Add objects to protect.
-- Example: add_protect({table, math, print})
function M.add_protect(object_array)
    for _, obj in pairs(object_array) do
        protected[obj] = true
    end
end  -- add_protect()

-- Remove objects in protected set.
-- Example: remove_protect({table, math, print})
function M.remove_protect(object_array)
    for _, obj in pairs(object_array) do
        protected[obj] = nil
    end
end  -- remove_protect()

return M
