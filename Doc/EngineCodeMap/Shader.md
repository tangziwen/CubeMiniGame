# CubeEngine/EngineSrc/Shader

## Role

Shader program loading, macro injection, mutation variants, and backend shader collection creation.

## Important Objects

- `ShaderProgram`: source paths, mutation flag, uniform/attribute caches, text preprocessing, reload, and backend `DeviceShaderCollection`.
- `ShaderMgr`: singleton cache keyed by vertex shader, fragment shader, and mutation flag; owns global shader macros.
- `shaderInfo`: cache key for shader variants.
