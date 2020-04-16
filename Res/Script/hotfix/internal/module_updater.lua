--- Updater to update loaded module.
-- Updating a table is to update metatable and sub-table of the old table,
--   and to update functions of the old table, keeping value fields.
-- Updating a function is to copy upvalues of old function to new function.
-- Functions will be replaced later after updating.

local M = {}

local update_table
local update_func

-- Updated signature set to prevent self-reference dead loop.
local updated_sig = {}

-- Map old function to new functions.
local updated_func_map = {}

-- Do not update and replace protected objects.
-- Set to hotfix.protected.
local protected = {}

-- Set to hotfix.log_debug.
function M.log_debug(msg_str) end

-- Check if function or table has been updated. Return true if updated.
local function check_updated(new_obj, old_obj, name, deep)
    local signature = string.format("new(%s) old(%s)",
        tostring(new_obj), tostring(old_obj))
    M.log_debug(string.format("%sUpdate %s: %s", deep, name, signature))

    if new_obj == old_obj then
        M.log_debug(deep .. "  Same")
        return true
    end
    if updated_sig[signature] then
        M.log_debug(deep .. "  Already updated")
        return true
    end
    updated_sig[signature] = true
    return false
end

-- Update new function with upvalues of old function.
-- Parameter name and deep are only for log.
function update_func(new_func, old_func, name, deep)
    assert("function" == type(new_func))
    assert("function" == type(old_func))
    if protected[old_func] then return end
    if check_updated(new_func, old_func, name, deep) then return end
    deep = deep .. "  "
    updated_func_map[old_func] = new_func

    -- Get upvalues of old function.
    local old_upvalue_map = {}
    for i = 1, math.huge do
        local name, value = debug.getupvalue(old_func, i)
        if not name then break end
        old_upvalue_map[name] = value
    end

    local function log_dbg(name, from, to)
        M.log_debug(string.format("%ssetupvalue %s: (%s) -> (%s)",
            deep, name, tostring(from), tostring(to)))
    end

    -- Update new upvalues with old.
    for i = 1, math.huge do
        local name, value = debug.getupvalue(new_func, i)
        if not name then break end
        local old_value = old_upvalue_map[name]
        if old_value then
            local type_old_value = type(old_value)
            if type_old_value ~= type(value) then
                debug.setupvalue(new_func, i, old_value)
                log_dbg(name, value, old_value)
            elseif type_old_value == "function" then
                update_func(value, old_value, name, deep)
            elseif type_old_value == "table" then
                update_table(value, old_value, name, deep)
                debug.setupvalue(new_func, i, old_value)
            else
                debug.setupvalue(new_func, i, old_value)
                log_dbg(name, value, old_value)
            end
        end  -- if old_value
    end  -- for i
end  -- update_func()

-- Compare 2 tables and update the old table. Keep the old data.
function update_table(new_table, old_table, name, deep)
    assert("table" == type(new_table))
    assert("table" == type(old_table))
    if protected[old_table] then return end
    if check_updated(new_table, old_table, name, deep) then return end
    deep = deep .. "  "

    -- Compare 2 tables, and update old table.
    for key, value in pairs(new_table) do
        local old_value = old_table[key]
        local type_value = type(value)
        if type_value ~= type(old_value) then
            old_table[key] = value
            M.log_debug(string.format("%sUpdate field %s: (%s) -> (%s)",
                deep, key, tostring(old_value), tostring(value)))
        elseif type_value == "function" then
            update_func(value, old_value, key, deep)
        elseif type_value == "table" then
            update_table(value, old_value, key, deep)
        end
    end  -- for

    -- Update metatable.
    local old_meta = debug.getmetatable(old_table)
    local new_meta = debug.getmetatable(new_table)
    if type(old_meta) == "table" and type(new_meta) == "table" then
        update_table(new_meta, old_meta, name.."'s Meta", deep)
    end
end  -- update_table()

-- Update new loaded object with package.loaded[module_name].
local function update_loaded_module2(module_name, new_obj)
    assert(nil ~= new_obj)
    assert("string" == type(module_name))
    local old_obj = package.loaded[module_name]
    local new_type = type(new_obj)
    local old_type = type(old_obj)
    if new_type == old_type then
        if "table" == new_type then
            update_table(new_obj, old_obj, module_name, "")
            return
        end
        if "function" == new_type then
            update_func(new_obj, old_obj, module_name, "")
            return;
        end
    end  -- if new_type == old_type
    M.log_debug(string.format("Directly replace module: old(%s) -> new(%s)",
        tostring(old_obj), tostring(new_obj)))
    package.loaded[module_name] = new_obj
end  -- update_loaded_module2()

-- Update new loaded object with package.loaded[module_name].
-- Return an updated function map (updated_func_map).
-- new_module_obj is the newly loaded module object.
function M.update_loaded_module(module_name, protected_objects, new_module_obj)
    assert(type(module_name) == "string")
    assert(type(protected_objects) == "table")

    protected = protected_objects
    updated_func_map = {}
    updated_sig = {}
    update_loaded_module2(module_name, new_module_obj)
    updated_sig = {}
    return updated_func_map
end  -- update_loaded_module()

return M
