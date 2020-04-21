--- Replace functions of table or upvalue.
-- Search for the old functions and replace them with new ones.

local M = {}

-- Objects whose functions have been replaced already.
-- Each objects need to be replaced once.
local replaced_obj = {}

-- Map old functions to new functions.
-- Used to replace functions finally.
-- Set to hotfix.updated_func_map.
local updated_func_map = {}

-- Do not update and replace protected objects.
-- Set to hotfix.protected.
local protected = {}

local replace_functions  -- forward declare

-- Replace all updated functions in upvalues of function object.
local function replace_functions_in_upvalues(function_object)
    local obj = function_object
    assert("function" == type(obj))
    assert(not protected[obj])
    assert(obj ~= updated_func_map)

    for i = 1, math.huge do
        local name, value = debug.getupvalue(obj, i)
        if not name then return end
        local new_func = updated_func_map[value]
        if new_func then
            assert("function" == type(value))
            debug.setupvalue(obj, i, new_func)
        else
            replace_functions(value)
        end
    end  -- for
    assert(false, "Can not reach here!")
end  -- replace_functions_in_upvalues()

-- Replace all updated functions in the table.
local function replace_functions_in_table(table_object)
    local obj = table_object
    assert("table" == type(obj))
    assert(not protected[obj])
    assert(obj ~= updated_func_map)
    
    replace_functions(debug.getmetatable(obj))
    local new = {}  -- to assign new fields
    for k, v in pairs(obj) do
        local new_k = updated_func_map[k]
        local new_v = updated_func_map[v]
        if new_k then
            obj[k] = nil  -- delete field
            new[new_k] = new_v or v
        else
            obj[k] = new_v or v
            replace_functions(k)
        end
        if not new_v then replace_functions(v) end
    end  -- for k, v
    for k, v in pairs(new) do obj[k] = v end
end  -- replace_functions_in_table()

-- Replace all updated functions.
-- Record all replaced objects in replaced_obj.
function replace_functions(obj)
    if protected[obj] then return end
    local obj_type = type(obj)
    if "function" ~= obj_type and "table" ~= obj_type then return end
    if replaced_obj[obj] then return end
    replaced_obj[obj] = true
    assert(obj ~= updated_func_map)

    if "function" == obj_type then
        replace_functions_in_upvalues(obj)
    else  -- table
        replace_functions_in_table(obj)
    end
end  -- replace_functions(obj)

--- Replace all old functions with new ones.
-- Replace in new_obj, _G and debug.getregistry().
-- a_protected is a list of protected object.
-- an_updated_func_map is a map from old function to new function.
-- new_obj is the newly loaded module.
function M.replace_all(a_protected, an_updated_func_map, new_obj)
    protected = a_protected
    updated_func_map = an_updated_func_map
    assert(type(protected) == "table")
    assert(type(updated_func_map) == "table")
    if nil == next(updated_func_map) then
        return
    end

    replaced_obj = {}
    replace_functions(new_obj)  -- new_obj may be not in _G
    replace_functions(_G)
    replace_functions(debug.getregistry())
    replaced_obj = {}
end  -- M.replace_all()

return M
