# CubeEngine/EngineSrc/Log

## Role

Legacy/simple log system facade.

## Important Objects

- `LogSystem`: singleton with formatted/string logging methods.
- `T_LOG`: macro alias for `LogSystem::shared()->log`.

## Boundaries

- Newer utility logging helpers live in `Utility/log/Log.h` as `tlog`, `tlogError`, and `initLogSystem()`.

## Known Traps

- There are two logging surfaces (`Log` and `Utility/log`); check existing call sites before standardizing a module.
