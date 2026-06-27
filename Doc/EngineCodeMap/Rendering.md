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

## Flow

- `GraphicsRenderer::preTick()` applies TSAA jitter before scene/render work.
- `GraphicsRenderer::render()` prepares backend frame/command buffer, collects scene queues, executes stages, then hands `RenderPath` to backend.
- GUI pass draws retained `Drawable2D` commands first, then calls `IMGUISystem::renderIMGUI()` and uploads ImGui draw data.

## Boundaries

- Stage creation and pass ordering live here; backend-specific command details belong in `BackEnd`.
- Scene objects submit `RenderCommand`s; they should not directly drive backend command buffers.

## Known Traps

- TSAA modifies the default camera pixel offset; disable paths must reset it.
- ImGui frame lifecycle is split: `IMGUISystem::NewFrame()` in `Engine::update()`, `renderIMGUI()` inside GUI render pass.
