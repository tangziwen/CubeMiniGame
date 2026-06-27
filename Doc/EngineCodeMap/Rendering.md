# CubeEngine/EngineSrc/Rendering

## Role

High-level render pipeline orchestration, render queues, post effects, and render-stage routing.

## Important Objects

- `GraphicsRenderer`: builds and runs the Vulkan-oriented frame graph: shadows, GBuffer, deferred lighting, sky, transparent, debug wireframe, HBAO/SSR/SSGI/fog/bloom/TSAA/outline, texture-to-screen, GUI/ImGui.
- `RenderPath`: ordered list of `DeviceRenderStage`s submitted to backend `endFrame()`.
- `RenderQueue`: stores `RenderCommand`s and batches instancing through `InstancingMgr`.
- `RenderCommand`: mesh/material/drawable transform packet with stage mask, primitive type, depth policy, batch type, and outline color.
- `RenderBuffer`: CPU/GPU buffer wrapper used by mesh and instance data.
- `TSAA`, `SSGI`, `Bloom`, `OutlinePass`: postprocess modules owned by `GraphicsRenderer`.
- `RenderFlag`, `ImageFormat`: shared render enums and image format mapping.
