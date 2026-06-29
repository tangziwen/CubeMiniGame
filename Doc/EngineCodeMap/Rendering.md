# CubeEngine/EngineSrc/Rendering

## Role

High-level render pipeline orchestration, 3D render views, render queues, post effects, draw-pass routing, and the first lightweight RenderGraph layer.

## Important Objects

- `GraphicsRenderer`: owns frame begin/end orchestration, schedules 3D/shadow systems, then handles texture-to-screen, GUI/ImGui, and thumbnails.
- `CSMShadowSystem`: owns cascaded shadow `ShadowView` instances, updates shadow projection state, draws cascades in order, and exposes depth textures for scene lighting.
- `DrawPass`: draw submission vocabulary (`DrawPassType`, `DrawPassTypeMask`) used by drawables, materials, render queues, render views, and device stages.
- `RenderGraph`: V1 graph runner for sequential graph passes; `RenderGraphPassDesc` binds an existing `DeviceRenderStage*` with an execute callback, and `RenderGraphContext` carries command/path/queue access.
- `RenderView`: base 3D view context with view type/index, camera, output queue, and ordered passes; it derives submit draw-pass masks from queue-consuming passes and does not own UI.
- `RenderViewPass`: per-view pass descriptor that declares the consumed `DrawPassTypeMask`; fullscreen/light/post passes may consume no scene queue.
- `SceneView`: main 3D view that owns the GBuffer, deferred lighting, transparent, sky, debug wireframe, HBAO/SSR/SSGI/bloom/TSAA/outline pipeline and currently runs fog through `RenderGraph`.
- `ShadowView`: per-CSM-cascade 3D view that owns the shadow-map pass and collects only shadow-casting 3D drawables.
- `RenderPath`: ordered list of `DeviceRenderStage`s submitted to backend `endFrame()`.
- `RenderQueue`: stores `RenderCommand`s and batches instancing through `InstancingMgr`.
- `RenderCommand`: mesh/material/drawable transform packet with draw-pass mask, primitive type, depth policy, batch type, and outline color.
- `RenderBuffer`: CPU/GPU buffer wrapper used by mesh and instance data.
- `TSAA`, `SSGI`, `Bloom`, `OutlinePass`: postprocess modules owned by `SceneView`.
- `RenderFlag`, `ImageFormat`: shared render enums and image format mapping.
