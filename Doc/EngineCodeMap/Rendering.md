# CubeEngine/EngineSrc/Rendering

## Role

High-level render pipeline orchestration, 3D render views, render queues, post effects, and render-stage routing.

## Important Objects

- `GraphicsRenderer`: owns frame begin/end orchestration, schedules 3D/shadow systems, then handles texture-to-screen, GUI/ImGui, and thumbnails.
- `CSMShadowSystem`: owns cascaded shadow `ShadowView` instances, updates shadow projection state, draws cascades in order, and exposes depth textures for scene lighting.
- `RenderView`: base 3D view context with view type/index, camera, output queue, and ordered passes; it derives submit stage masks from passes and does not own UI.
- `RenderViewPass`: per-view pass descriptor that declares the consumed `RenderStage`; fullscreen/light/post passes may consume no scene queue.
- `SceneView`: main 3D view that owns the GBuffer, deferred lighting, transparent, sky, debug wireframe, HBAO/SSR/SSGI/fog/bloom/TSAA/outline pipeline.
- `ShadowView`: per-CSM-cascade 3D view that owns the shadow-map pass and collects only shadow-casting 3D drawables.
- `RenderPath`: ordered list of `DeviceRenderStage`s submitted to backend `endFrame()`.
- `RenderQueue`: stores `RenderCommand`s and batches instancing through `InstancingMgr`.
- `RenderCommand`: mesh/material/drawable transform packet with stage mask, primitive type, depth policy, batch type, and outline color.
- `RenderBuffer`: CPU/GPU buffer wrapper used by mesh and instance data.
- `TSAA`, `SSGI`, `Bloom`, `OutlinePass`: postprocess modules owned by `SceneView`.
- `RenderFlag`, `ImageFormat`: shared render enums and image format mapping.
