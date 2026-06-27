# CubeEngine/EngineSrc/Utility

## Role

File/data helpers, JSON helpers, logging helpers, string/misc/math utilities, and stack trace support.

## Important Objects

- `Tfile`: search path and zip-backed resource lookup, file data loading, relative/absolute path helpers, JSON loading, extension/name utilities.
- `Data`: byte buffer owner with copy/move/fastSet semantics.
- `JsonUtility`: RapidJSON helpers for `vec3`/`vec4`.
- `tlog`, `tlogError`, `initLogSystem`: practical logging API used by much of the engine.
- `Tmisc`: string conversion, formatting, duration helpers, clamp, user path helper.
- `TbaseMath`: random engine, constants, angle conversion, power-of-two, clamp helpers.
- `Tstring`, `Tvector`: older custom string/vector helpers.
- `TstatckTrace`: stack trace utility.

## Contracts

- `Engine::preSetting()` initializes common `Tfile` search paths and zip package.
- `Data::fastSet()` transfers ownership of malloc/calloc memory to `Data`.

## Boundaries

- Keep utility helpers independent from scene/render/gameplay ownership.

## Known Traps

- Search-path order matters for resource bugs.
- `Tfile` can read from both filesystem and zip; always log resolved paths when debugging loading failures.
