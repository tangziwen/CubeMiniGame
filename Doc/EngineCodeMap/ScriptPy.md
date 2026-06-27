# CubeEngine/EngineSrc/ScriptPy

## Role

Python embedding bridge for script initialization, UI update, input events, and typed function calls.

## Important Objects

- `ScriptPyMgr`: singleton Python lifecycle, script init/update, input event forwarding, reload/finalize, and templated Python function calls.
- `ScriptBinding.cpp` / `ScriptBindMacro.h`: C++ binding helpers/macros.
- `ScriptUI`: script-facing UI glue.

## Contracts

- `Engine::onStart()` calls `ScriptPyMgr::init()` and `doScriptInit()`.
- `EventMgr` forwards key/mouse/scroll events to `ScriptPyMgr::raiseInputEvent()` after dispatch.
- Template call helpers convert basic C++ types into Python objects.

## Boundaries

- Python code should be an extension layer; core engine state should remain owned by C++ systems.

## Known Traps

- Python reference ownership is manual; check `Py_DECREF` / stolen-reference behavior before editing call helpers.
