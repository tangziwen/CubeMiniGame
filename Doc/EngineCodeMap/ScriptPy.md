# CubeEngine/EngineSrc/ScriptPy

## Role

Python embedding bridge for script initialization, UI update, input events, and typed function calls.

## Important Objects

- `ScriptPyMgr`: singleton Python lifecycle, script init/update, input event forwarding, reload/finalize, and templated Python function calls.
- `ScriptBinding.cpp` / `ScriptBindMacro.h`: C++ binding helpers/macros.
- `ScriptUI`: script-facing UI glue.
