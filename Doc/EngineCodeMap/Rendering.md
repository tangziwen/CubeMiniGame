# CubeEngine/EngineSrc/Rendering

## Role

High-level render pipeline orchestration, 3D render views, render queues, post effects, draw-pass routing, and the lightweight RenderGraph node/resource layer.

## Important Objects

- `GraphicsRenderer`: owns the shared frame `RenderGraph`, frame begin/end, CSM and scene-view scheduling, screen composition, GUI/ImGui graph nodes, and the final Present node; `createSceneView(camera, size)` adds an offscreen scene view, while thumbnails still use the separate legacy stage.
- `CSMShadowSystem`: updates cascade projections and collects `ShadowView`s; `buildRenderGraph()` returns depth-output nodes consumed by scene lighting. Current cascade coverage is computed from the default scene camera and shared by scene views.
- `DrawPass`: draw submission vocabulary (`DrawPassType`, `DrawPassTypeMask`) used by drawables, materials, render queues, render views, and device stages.
- `RenderGraph`: authors and validates dependencies and texture states for raster/compute/blit/Present nodes; owns empty/uploaded textures, compute shaders, framebuffer/pass caches and indexed-draw uploads, and executes backend stages internally. Empty textures start in `Undefined` state with uninitialized contents; `createTexture(desc)` and `createComputeShader(path)` bridge resource creation to the generic backend. `RenderGraphPassContext::bindItemUniform()` copies and binds per-draw data; `RenderGraphResourceHandle::operator->()` resolves typed resources and `handle->get<DeviceTexture>()` / `handle->get<DeviceFrameBuffer>()` expose the generic backend object directly. Resource types are checked; generation tracking invalidates handles after `clearResources()` and registry growth does not invalidate live handles. Passes may borrow generic `Device*`; resource ownership and access tracking remain with the graph. `addIndexedRasterNode()` accepts neutral vertex layouts, 16/32-bit index streams, per-draw textures/scissors and callbacks; sampled textures join graph access validation automatically. Raster passes carry their own queue and camera, and depth-only nodes expose depth as their default output.
- `RenderView`: base view context with type/index, camera, private render queue and shared graph reference; `buildRenderGraph()` contributes nodes and an output without clearing, compiling or executing the whole frame graph.
- `SceneView`: holds graph handles for view-local GBuffer/postprocess resources and TSAA history, accepts a camera and target size, and contributes the deferred scene/Bloom/Outline chain to the shared graph. It returns an offscreen output node; screen composition belongs to `GraphicsRenderer`.
- `ShadowView`: per-CSM-cascade view that collects shadow casters and contributes a graph-owned depth-only raster pass; lighting samples its graph resource directly.
- `RenderPath`: ordered list of `DeviceRenderStage`s submitted to backend `endFrame()`.
- `RenderQueue`: stores `RenderCommand`s and batches instancing through `InstancingMgr`.
- `RenderCommand`: mesh/material/drawable transform packet with draw-pass mask, primitive type, depth policy, batch type, and outline color.
- `RenderBuffer`: CPU/GPU buffer wrapper used by mesh and instance data.
- `TSAA`, `SSGI`, `Bloom`, `OutlinePass`: postprocess modules owned by `SceneView`; TSAA/SSGI use the pass camera and postprocess dimensions follow the view target. Bloom holds graph texture handles and borrowed graph-owned compute shaders; its scene input and intermediate images use `RGBA16_Float` to match `rgba16f` shader declarations.
- `RenderFlag`, `ImageFormat`, `VertexLayout`: shared rendering flags, API-independent image formats with explicit channel precision/UNorm/Float semantics, and neutral vertex attribute descriptions. `ImageFormat` covers RGB/RGBA 8/16-bit and D16/D32/depth-stencil formats; Vulkan format conversion stays in the backend.
