# CubeEngine/EngineSrc/BackEnd

## Role

Platform/window bridge and graphics backend abstraction. Current production path is Vulkan.

## Important Objects

- `RenderBackEndBase`: abstract factory and frame interface for textures, shaders, buffers, render passes, pipelines, stages, framebuffers, materials, frame begin/end, current frame command, swapchain framebuffer access and wireframe capability queries. `createTexture_imp(DeviceTextureDesc)` creates a caller-owned empty texture in Undefined state.
- `VKRenderBackEnd`: Vulkan device/swapchain/descriptor/command/pipeline owner and concrete backend factory; maps `ImageFormat` to supported Vulkan formats and validates empty texture format/usage/extent.
- `RenderBackEnd`: older GL-style facade plus fallback factory surface; still appears in config/debug paths.
- `WindowBackEnd` / `WindowBackEndMgr` / `GLFW_BackEnd`: window loop and platform input source.
- `AbstractDevice`: translates GLFW/platform input into `EventMgr`, owns delta timing, resize, and backend creation.
- `DeviceRenderStage`: backend-neutral stage interface for render/compute passes, binding, draw calls, screen quad/sphere helpers, per-stage pipeline/material caches, neutral vertex layouts for default and custom pipelines, generic backend factories, 16/32-bit indexed draws and view camera propagation to material uniforms.
- `DevicePipeline`, `DeviceMaterial`, `DeviceDescriptor`, `DeviceBuffer`, `DeviceTexture`, `DeviceFrameBuffer`, `DeviceRenderPass`, `DeviceRenderCommand`: backend resource abstractions.
- `DeviceItemBuffer` in `DeviceBuffer.h`: borrowed buffer/offset/size slice; the pool stays in the backend. `DeviceDescriptor::updateUniformByBinding()` copies CPU data into backend-owned storage for the current submission.
- `DeviceShaderBindingInfo`: generic shader reflection with engine-defined stage masks; Vulkan shader collections own their shader modules and descriptor layouts. Graph-owned compute shaders are released after cached stages.
- `BackEnd/vk/*`: Vulkan implementations, descriptor-set layout, item-buffer pool, memory pool, shader collection, material, pipeline, render pass/stage/command, texture, framebuffer.
- `BackEnd/gl/*`: legacy GL device resources.
