# CubeEngine/EngineSrc/BackEnd

## Role

Platform/window bridge and graphics backend abstraction. Current production path is Vulkan.

## Important Objects

- `RenderBackEndBase`: abstract factory and frame interface for textures, shaders, buffers, render passes, pipelines, stages, framebuffers, materials, and frame begin/end.
- `VKRenderBackEnd`: Vulkan device/swapchain/descriptor/command/pipeline owner and concrete backend factory.
- `RenderBackEnd`: older GL-style facade plus fallback factory surface; still appears in config/debug paths.
- `WindowBackEnd` / `WindowBackEndMgr` / `GLFW_BackEnd`: window loop and platform input source.
- `AbstractDevice`: translates GLFW/platform input into `EventMgr`, owns delta timing, resize, and backend creation.
- `DeviceRenderStage`: backend-neutral stage interface for render/compute passes, binding, draw calls, screen quad/sphere helpers, and per-stage pipeline/material caches.
- `DevicePipeline`, `DeviceMaterial`, `DeviceDescriptor`, `DeviceBuffer`, `DeviceTexture`, `DeviceFrameBuffer`, `DeviceRenderPass`, `DeviceRenderCommand`: backend resource abstractions.
- `BackEnd/vk/*`: Vulkan implementations, descriptor-set layout, item-buffer pool, memory pool, shader collection, material, pipeline, render pass/stage/command, texture, framebuffer.
- `BackEnd/gl/*`: legacy GL device resources.

## Flow

- `Engine::run()` creates a `WindowBackEnd`; `AbstractDevice::createRenderBackEnd()` chooses the render backend.
- `GraphicsRenderer` asks backend factories for all render resources and submits a `RenderPath` every frame.
- Vulkan item-wise uniforms use `DeviceItemBufferPoolVK` and per-pipeline item descriptor sets.

## Boundaries

- Keep Vulkan types inside `VKRenderBackEnd` and `BackEnd/vk` except where unavoidable glue already exists.
- High-level pass ordering belongs to `Rendering`, not backend classes.

## Known Traps

- `RenderBackEnd` and `VKRenderBackEnd` coexist; verify call sites before assuming one abstraction path.
- `DeviceRenderStage::getName()` currently returns `std::string&` but does not explicitly return in the header; check implementation/compiler behavior before touching.
