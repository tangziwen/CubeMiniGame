# CubeEngine/EngineSrc/Shader

## Role

Shader program loading, macro injection, mutation variants, and backend shader collection creation.

## Important Objects

- `ShaderProgram`: source paths, mutation flag, uniform/attribute caches, text preprocessing, reload, and backend `DeviceShaderCollection`.
- `ShaderMgr`: singleton cache keyed by vertex shader, fragment shader, and mutation flag; owns global shader macros.
- `shaderInfo`: cache key for shader variants.

## Contracts

- Request shaders through `ShaderMgr::getByPath()` when sharing/reload behavior matters.
- `ShaderProgram::reload()` rebuilds backend shader resources from stored paths.
- `ShaderMgr::reloadAllShaders()` is the broad hot-reload entry.

## Boundaries

- Shader module creation is delegated to backend `DeviceShaderCollection`.
- Material decides which shader paths and mutation flags to request.

## Known Traps

- Cache key currently centers on VS/FS/mutation; tessellation paths should be verified before relying on variant uniqueness.
