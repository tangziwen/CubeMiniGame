# CubeEngine/EngineSrc/Rendering

## Role

High-level render pipeline orchestration, 3D render views, render queues, post effects, draw-pass routing, and the lightweight RenderGraph node/resource layer.

## Important Objects

- `GraphicsRenderer`: owns frame begin/end orchestration, schedules shadow and `SceneView`, then handles GUI/ImGui and thumbnails after `SceneView` writes the texture-to-screen result into the swapchain framebuffer.
- `CSMShadowSystem`: owns cascaded shadow `ShadowView` instances, updates shadow projection state, draws cascades in order, and exposes depth textures for scene lighting.
- `DrawPass`: draw submission vocabulary (`DrawPassType`, `DrawPassTypeMask`) used by drawables, materials, render queues, render views, and device stages.
- `RenderGraph`: graph node/resource authoring and execution layer; `RenderGraphNode` expresses pass dependencies and optional outputs, `compile(root)` builds a dependency-first order from the final node, `validate()` checks resource declarations plus compiled writer/consumer dependency paths, `RenderGraphResourceHandle` names textures/framebuffers/storage-image texture access, and backend `DeviceRenderStage`s are compiled/executed internally for raster, compute, blit, and external nodes before being added to `RenderPath`.
- `RenderView`: base 3D view context with view type/index, camera, output queue, and ordered passes; it derives submit draw-pass masks from queue-consuming passes and does not own UI.
- `RenderViewPass`: per-view pass descriptor that declares the consumed `DrawPassTypeMask`; fullscreen/light/post passes may consume no scene queue.
- `SceneView`: main 3D view authored through `RenderGraph` nodes; it builds the scene chain each frame, keeps GBuffer/deferred/AfterDepthClear/transparent/sky/debug/HBAO/SSR/Fog/TSAA/TextureToScreen as graph-owned raster/fullscreen nodes, runs Bloom Bright Pass as a graph-owned compute node, uses SceneColorCopy as a blit node, conditionally inserts Outline as an external module node only when outline commands exist, and compiles from the final texture-to-screen node before executing.
- `ShadowView`: per-CSM-cascade 3D view that owns the shadow-map pass and collects only shadow-casting 3D drawables.
- `RenderPath`: ordered list of `DeviceRenderStage`s submitted to backend `endFrame()`.
- `RenderQueue`: stores `RenderCommand`s and batches instancing through `InstancingMgr`.
- `RenderCommand`: mesh/material/drawable transform packet with draw-pass mask, primitive type, depth policy, batch type, and outline color.
- `RenderBuffer`: CPU/GPU buffer wrapper used by mesh and instance data.
- `TSAA`, `SSGI`, `Bloom`, `OutlinePass`: postprocess modules owned by `SceneView`.
- `RenderFlag`, `ImageFormat`: shared render enums and image format mapping.
