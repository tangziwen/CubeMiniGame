#include "BackEnd/DeviceFrameBuffer.h"
#include "BackEnd/DeviceRenderCommand.h"
#include "BackEnd/DeviceTexture.h"
#include "BackEnd/DeviceRenderStage.h"
#include "BackEnd/RenderBackEndBase.h"
#include "Base/Camera.h"
#include "Engine/Engine.h"
#include "Technique/MaterialInstance.h"
#include <cstring>
#include <limits>
#include "Rendering/RenderGraph.h"

#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace tzw
{
class RenderGraphTestAccess
{
public:
	static void markGraphOwnedAndUninitialized(RenderGraph& graph, RenderGraphResourceHandle handle)
	{
		auto& resource = graph.m_resources[handle.index()];
		resource.desc.imported = false;
		resource.contentsInitialized = false;
	}

	static RenderGraphPassDesc& pass(RenderGraph& graph, RenderGraphNode node)
	{
		return graph.m_passes[node.passHandle().index()];
	}

	static bool finishRasterPass(RenderGraph& graph, RenderGraphContext& context, RenderGraphResourceHandle target,
		DeviceRenderPass* renderPass, const std::vector<RenderGraphResourceAccess>& accesses, std::string& message)
	{
		RenderGraphPassDesc pass;
		pass.name = "RasterFinalState";
		pass.kind = RenderGraphPassKind::Raster;
		pass.renderPass = renderPass;
		pass.frameBufferResource = target;
		pass.resourceAccesses = accesses;
		return graph.updateResourceStatesAfterPass(context, pass, message);
	}

	static DeviceTextureState colorState(const RenderGraph& graph, RenderGraphResourceHandle handle)
	{
		return graph.m_resources[handle.index()].currentColorState;
	}
};

namespace
{
class FakeTexture final : public DeviceTexture
{
public:
    explicit FakeTexture(int* destroyed = nullptr) : destroyed(destroyed) {}
    ~FakeTexture() override { if(destroyed) ++*destroyed; }
    int* destroyed;
};

class FakeFrameBuffer final : public DeviceFrameBuffer
{
public:
	FakeFrameBuffer(std::vector<DeviceTexture*> colors, DeviceTexture* depth = nullptr, vec2 size = vec2(64.0f, 64.0f))
	{
		m_textureList = std::move(colors);
		if(depth)
		{
			m_textureList.emplace_back(depth);
		}
		m_depthTexture = depth;
		m_size = size;
	}

	void init(int width, int height, DeviceRenderPass*) override
	{
		m_size = vec2(static_cast<float>(width), static_cast<float>(height));
	}

	void initWithTextures(DeviceRenderPass*, const std::vector<DeviceTexture*>& textureList, int width, int height) override
	{
		m_textureList = textureList;
		m_size = vec2(static_cast<float>(width), static_cast<float>(height));
	}

	void init(DeviceTexture* texture, DeviceTexture* depth, DeviceRenderPass*) override
	{
		m_textureList.clear();
		if(texture)
		{
			m_textureList.emplace_back(texture);
		}
		if(depth)
		{
			m_textureList.emplace_back(depth);
		}
		m_depthTexture = depth;
	}

	DeviceTexture* getDepthMap() override
	{
		return m_depthTexture;
	}

	std::vector<DeviceTexture*>& getTextureList() override
	{
		return m_textureList;
	}

	vec2 getSize() override
	{
		return m_size;
	}
};

class FakeRenderPass final : public DeviceRenderPass
{
public:
	void init(const DeviceAttachmentInfoList& attachments, OpType opType, bool transitionToRead, bool outputToScreen = false) override
	{
		m_attachmentList = attachments;
		m_opType = opType;
		m_isNeedTransitionToRead = transitionToRead;
		m_isOutPutToScreen = outputToScreen;
	}
};

class FakeRenderCommand final : public DeviceRenderCommand
{
public:
	FakeRenderCommand()
		: DeviceRenderCommand(nullptr)
	{
	}

	void startRecord() override
	{
	}

	void endRecord() override
	{
	}

	bool textureBarrier(const DeviceTextureBarrier& barrier) override
	{
		barriers.emplace_back(barrier);
		return acceptBarriers;
	}

	bool blitTexture(const DeviceTextureBlit& blit) override
	{
		blits.emplace_back(blit);
		return acceptBlits;
	}

	bool acceptBarriers = true;
	bool acceptBlits = true;
	std::vector<DeviceTextureBarrier> barriers;
	std::vector<DeviceTextureBlit> blits;
};

class FakeBuffer final : public DeviceBuffer
{
public:
	void allocate(void* data, size_t size) override
	{
		const auto bytes = static_cast<const uint8_t*>(data);
		upload.assign(bytes, bytes + size);
	}
	bool init(DeviceBufferType type) override { m_type = type; return true; }
	void bind() override {}
	void setUsePool(bool) override {}
	std::vector<uint8_t> upload;
};

class FakeDescriptor final : public DeviceDescriptor
{
public:
	void updateDescriptorByBinding(int, DeviceTexture*) override {}
	void updateDescriptorByBinding(int, std::vector<DeviceTexture*>&) override {}
	void updateDescriptorByBinding(int, DeviceBuffer*, size_t, size_t) override {}
	void updateDescriptorByBinding(int, DeviceItemBuffer*) override {}
	void updateDescriptorByBindingAsStorageImage(int, DeviceTexture*) override {}
    bool updateUniformByBinding(int binding, const void* data, size_t size) override
    {
        if(!data || !size) return false;
        uniformBinding = binding;
        auto bytes = static_cast<const uint8_t*>(data);
        uniform.assign(bytes, bytes + size);
        return true;
    }
    int uniformBinding = -1;
    std::vector<uint8_t> uniform;
};

class FakeMaterial final : public DeviceMaterial
{
public:
    DeviceDescriptor* getMaterialDescriptorSet() override { return &descriptor; }
    void updateUniform() override {}
    void initCompute(DeviceShaderCollection*) override {}
    void init(MaterialInstance*, MaterialTechniqueType) override {}
    void updateUniformSingle(std::string, void*, size_t) override {}
    void updateMaterialDescriptorSet() override {}
    FakeDescriptor descriptor;
};

class FakePipeline final : public DevicePipeline
{
public:
	void initCompute(DeviceShaderCollection*) override {}
	void init(vec2, MaterialInstance*, DeviceRenderPass*, VertexLayout, bool, VertexLayout, int,
		MaterialTechniqueType) override {}
	void resetItemWiseDescritporSet() override {}
	DeviceDescriptor* giveItemWiseDescriptorSet() override { return &descriptor; }
	FakeDescriptor descriptor;
};

class FakeStage final : public DeviceRenderStage
{
public:
	void finish() override {}
	void draw(RenderQueue* queue, MaterialTechniqueType) override { lastQueue = queue; }
	void drawScreenQuad() override {}
	void drawSphere() override {}
	void dispatch(unsigned int, unsigned int, unsigned int) override {}
	void beginCompute() override {}
	void endCompute() override {}
	void createSinglePipeline(MaterialInstance*, const VertexLayout&, bool) override { m_singlePipeline = new FakePipeline(); }
	void bindSinglePipelineDescriptor() override {}
	void bindSinglePipelineDescriptor(DeviceDescriptor* descriptor) override
    {
        auto fake = static_cast<FakeDescriptor*>(descriptor);
        uniformBindings.push_back(fake->uniformBinding);
        uniformUploads.push_back(fake->uniform);
    }
	void bindSinglePipelineDescriptorCompute() override {}
	void bindPipeline(DevicePipeline*) override {}
	void bindDescriptor(DevicePipeline*, std::vector<DeviceDescriptor*>) override {}
	void beginRenderPass(DeviceFrameBuffer*, vec4, vec2) override { observedCamera = m_viewCamera; }
	void endRenderPass() override {}
	void bindVBO(DeviceBuffer*) override {}
	void bindIBO(DeviceBuffer*, bool use32BitIndices) override { index32 = use32BitIndices; }
	void setScissor(vec4 rect) override { scissor = rect; }
	void drawElement(uint32_t count, uint32_t, uint32_t first, int32_t offset, uint32_t) override
	{
		drawCount++;
		lastIndexCount = count;
		lastFirstIndex = first;
		lastVertexOffset = offset;
	}
	std::vector<int> uniformBindings;
    std::vector<std::vector<uint8_t>> uniformUploads;
	RenderQueue* lastQueue = nullptr;
	Camera* observedCamera = nullptr;
	bool index32 = false;
	int drawCount = 0;
	uint32_t lastIndexCount = 0, lastFirstIndex = 0;
	int32_t lastVertexOffset = 0;
	vec4 scissor;
protected:
	void fetchCommand() override {}
};

class FakeBackend final : public RenderBackEndBase
{
public:
	FakeBackend() : previous(Engine::shared()->getRenderBackEnd()) { Engine::shared()->setRenderBackEnd(this); }
	~FakeBackend() { Engine::shared()->setRenderBackEnd(previous); }
	DeviceTexture* loadTextureRaw_imp(const unsigned char*, int, int, ImageFormat, unsigned int) override { return new FakeTexture(); }
	DeviceTexture* createTexture_imp(const DeviceTextureDesc& desc) override
    {
        lastTextureDesc = desc;
        auto texture = new FakeTexture(&destroyedTextures);
        texture->m_metaInfo.width = desc.width;
        texture->m_metaInfo.height = desc.height;
        texture->m_metaInfo.m_imageFormat = desc.format;
        texture->setTextureRole(desc.role);
        texture->setTextureUsage(desc.usage);
        return texture;
    }
    DeviceShaderCollection* createShader_imp() override { return nullptr; }
	DeviceBuffer* createBuffer_imp() override { return new FakeBuffer(); }
	DeviceRenderPass* createDeviceRenderpass_imp() override { return new FakeRenderPass(); }
	DevicePipeline* createPipeline_imp() override { return new FakePipeline(); }
    DeviceMaterial* createDeviceMaterial_imp() override { return new FakeMaterial(); }
	DeviceRenderStage* createRenderStage_imp() override
	{
		auto stage = new FakeStage();
		stages.emplace_back(stage);
		return stage;
	}
	DeviceFrameBuffer* createFrameBuffer_imp() override { return nullptr; }
	DeviceFrameBuffer* getSwapChainFrameBuffer(unsigned int) override { return swapchain; }
	void prepareFrame() override {}
	void endFrame(RenderPath*) override {}
	DeviceTextureDesc lastTextureDesc;
    int destroyedTextures = 0;
	RenderBackEndBase* previous;
	DeviceFrameBuffer* swapchain = nullptr;
	std::vector<FakeStage*> stages;
};

int failureCount = 0;

void expect(bool condition, const std::string& message)
{
	if(condition)
	{
		return;
	}
	failureCount++;
	std::cerr << "FAILED: " << message << '\n';
}

RenderGraphResourceDesc textureDesc(const std::string& name)
{
	RenderGraphResourceDesc desc;
	desc.name = name;
	desc.format = ImageFormat::RGBA8_UNorm;
	desc.role = TextureRoleEnum::AS_COLOR;
	desc.size = vec2(64.0f, 64.0f);
	desc.initialColorLayout = RenderGraphResourceLayout::ShaderRead;
	return desc;
}

RenderGraphResourceHandle importTexture(RenderGraph& graph, FakeTexture& texture, const std::string& name)
{
	return graph.importTexture(textureDesc(name), &texture);
}

RenderGraphNode addReadRoot(RenderGraph& graph, RenderGraphResourceHandle resource, const std::string& name = "Root")
{
	RenderGraphPassDesc desc;
	desc.name = name;
	desc.readColor(resource);
	return graph.addExternalNode(desc);
}

void testTypedResourceHandles()
{
    FakeBackend backend;
    RenderGraph graph;
    FakeTexture texture;
    auto handle = importTexture(graph, texture, "TypedTexture");
    expect(handle->type() == RenderGraphResourceKind::Texture && handle->get<DeviceTexture>() == &texture,
        "resource exposes its type and borrowed device object");
    expect(handle->get<DeviceFrameBuffer>() == nullptr, "resource type mismatch must return null");
    FakeFrameBuffer buffer({&texture});
    auto frame = graph.importFrameBuffer(textureDesc("TypedFrame"), &buffer);
    expect(frame->get<DeviceFrameBuffer>() == &buffer && frame->get<DeviceTexture>() == nullptr,
        "framebuffer resources expose only their matching device type");
    // Force registry growth; a handle must resolve the resource rather than cache a vector element address.
    for(int i = 0; i < 64; ++i) graph.createTexture(textureDesc("Scratch"));
    expect(handle->get<DeviceTexture>() == &texture, "resource handles survive registry reallocation");
    RenderGraph other;
    expect(other.resource(handle) == nullptr, "a foreign graph must reject the handle");
    graph.clearResources();
    auto replacement = importTexture(graph, texture, "Replacement");
    expect(replacement.isValid() && !handle.isValid() && handle->type() == RenderGraphResourceKind::Invalid
        && handle->get<DeviceTexture>() == nullptr, "clearing resources invalidates old handles even when indices are reused");
    expect(RenderGraphResourceHandle::invalid()->get<DeviceTexture>() == nullptr,
        "invalid resource handles safely resolve to null device objects");
}

void testOwnedEmptyTexture()
{
    FakeBackend backend;
    RenderGraph graph;
    auto desc = textureDesc("BloomScratch");
    desc.format = ImageFormat::RGBA16_Float;
    auto scratch = graph.createTexture(desc);
    expect(scratch.isValid() && backend.lastTextureDesc.format == ImageFormat::RGBA16_Float,
        "empty texture factory must preserve the requested HDR format");
    expect(graph.resource(scratch)->ownsTexture && !graph.resource(scratch)->contentsInitialized,
        "graph owns empty textures without pretending their contents are initialized");
    auto read = addReadRoot(graph, scratch);
    std::string message;
    expect(!graph.compile(read, &message), "empty texture must reject reads before its first write");
    graph.beginBuild();
    FakeTexture source;
    auto sourceHandle = importTexture(graph, source, "Source");
    auto copy = graph.addBlitNode("Initialize", sourceHandle, scratch);
    auto root = addReadRoot(graph, scratch).dependsOn(copy);
    expect(graph.compile(root, &message), "first write then read must compile: " + message);
    FakeRenderCommand command;
    RenderGraphContext context(&command, nullptr, nullptr);
    expect(graph.execute(context, &message), "empty texture initialization must execute: " + message);
    bool initializedFromUndefined = false;
    for(const auto& barrier : command.barriers)
    {
        if(barrier.texture == scratch->get<DeviceTexture>()
            && barrier.before.layout == DeviceTextureLayout::Undefined
            && barrier.before.usage == DeviceTextureUsage::None
            && barrier.after.layout == DeviceTextureLayout::TransferDst) initializedFromUndefined = true;
    }
    expect(initializedFromUndefined, "first GPU write must transition the new texture from Undefined");
    graph.clearResources();
    expect(backend.destroyedTextures == 1, "clearing graph must destroy owned texture exactly once");
    graph.clearResources();
    expect(backend.destroyedTextures == 1, "repeated clear must not destroy borrowed or already released textures");
}

void testNeutralStageAndUniformUpload()
{
    FakeBackend backend;
    RenderGraph graph;
    FakeTexture color;
    FakeFrameBuffer frameBuffer({&color});
    auto target = graph.importFrameBuffer(textureDesc("UniformTarget"), &frameBuffer);
    RenderGraphRasterPassDesc desc;
    desc.name = "UniformUploads";
    desc.frameBufferResource = target;
    desc.attachments = {{ImageFormat::RGBA8_UNorm, false}};
    desc.opType = DeviceRenderPass::OpType::LOADCLEAR_AND_STORE;
    MaterialInstance material;
    desc.material = &material;
    RenderGraphPassDesc accesses;
    accesses.writeColor(graph.colorAttachment(target));
    desc.resourceAccesses = accesses.resourceAccesses;
    auto node = graph.addFullscreenNode(desc, [](RenderGraphPassContext& context)
    {
        uint32_t value = 17;
        expect(context.bindItemUniform(3, &value, sizeof(value)), "first uniform upload must succeed");
        value = 29;
        expect(context.bindItemUniform(3, &value, sizeof(value)), "second draw gets its own uploaded value");
        expect(!context.bindItemUniform(3, nullptr, sizeof(value)), "null uniforms must be rejected");
        expect(!context.bindItemUniform(3, &value, 0), "empty uniforms must be rejected");
    });
    std::string message;
    expect(graph.compile(node, &message), "neutral full-screen pass must compile: " + message);
    FakeRenderCommand command;
    RenderGraphContext context(&command, nullptr, nullptr);
    expect(graph.execute(context, &message), "common stage must work with a non-Vulkan backend: " + message);
    auto stage = backend.stages.back();
    expect(stage->uniformUploads.size() == 2 && stage->uniformBindings == std::vector<int>({3, 3}),
        "uniform bindings must reach the backend once per valid upload");
    if(stage->uniformUploads.size() == 2)
    {
        uint32_t first = 0, second = 0;
        std::memcpy(&first, stage->uniformUploads[0].data(), sizeof(first));
        std::memcpy(&second, stage->uniformUploads[1].data(), sizeof(second));
        expect(first == 17 && second == 29, "uploads must snapshot CPU data rather than retain stack pointers");
    }
}

void testBasicImageFormats()
{
    expect(ImageFormatGetSize(ImageFormat::RGB8_UNorm) == 3
        && ImageFormatGetSize(ImageFormat::RGBA8_UNorm) == 4
        && ImageFormatGetSize(ImageFormat::RGB16_UNorm) == 6
        && ImageFormatGetSize(ImageFormat::RGBA16_Float) == 8, "RGB/RGBA byte sizes must preserve channel precision");
    expect(ImageFormatGetSize(ImageFormat::D16_UNorm) == 2
        && ImageFormatGetSize(ImageFormat::D32_Float) == 4, "D16 and D32 must report their actual byte sizes");
    expect(ImageFormatIsDepth(ImageFormat::D16_UNorm) && ImageFormatIsDepth(ImageFormat::D32_Float)
        && !ImageFormatHasStencil(ImageFormat::D16_UNorm) && !ImageFormatHasStencil(ImageFormat::D32_Float)
        && ImageFormatHasStencil(ImageFormat::D24_UNorm_S8_UInt), "depth-only formats must not acquire a stencil aspect");
}

void testDependencyCycle()
{
	RenderGraph graph;
	RenderGraphPassDesc firstDesc;
	firstDesc.name = "First";
	RenderGraphPassDesc secondDesc;
	secondDesc.name = "Second";
	auto first = graph.addExternalNode(firstDesc);
	auto second = graph.addExternalNode(secondDesc);
	first.dependsOn(second);
	second.dependsOn(first);
	std::string message;
	expect(!graph.compile(first, &message), "dependency cycle must fail compilation");
	expect(message.find("cycle") != std::string::npos, "cycle error should identify the cycle");
}

void testInvalidResourceHandle()
{
	RenderGraph graph;
	auto root = addReadRoot(graph, RenderGraphResourceHandle::invalid());
	std::string message;
	expect(!graph.compile(root, &message), "invalid resource handle must fail compilation");
	expect(message.find("invalid resource handle") != std::string::npos, "invalid handle error should be actionable");
}

void testGraphOwnedReadBeforeWrite()
{
	RenderGraph graph;
	FakeTexture texture;
	auto resource = importTexture(graph, texture, "Owned");
	RenderGraphTestAccess::markGraphOwnedAndUninitialized(graph, resource);
	auto root = addReadRoot(graph, resource);
	std::string message;
	expect(!graph.compile(root, &message), "graph-owned read before write must fail compilation");
	expect(message.find("before any reachable writer") != std::string::npos, "read-before-write error should identify the missing writer");
}

void testMissingRawDependency()
{
	RenderGraph graph;
	FakeTexture source;
	FakeTexture shared;
	FakeTexture output;
	auto sourceHandle = importTexture(graph, source, "Source");
	auto sharedHandle = importTexture(graph, shared, "Shared");
	auto outputHandle = importTexture(graph, output, "Output");
	auto writer = graph.addBlitNode("Writer", sourceHandle, sharedHandle);
	auto reader = graph.addBlitNode("Reader", sharedHandle, outputHandle);
	auto root = addReadRoot(graph, outputHandle);
	root.dependsOn(writer);
	root.dependsOn(reader);
	std::string message;
	expect(!graph.compile(root, &message), "RAW hazard without a dependency path must fail compilation");
	expect(message.find("after writer") != std::string::npos, "RAW error should identify the prior writer");
}

void testMissingWarDependency()
{
	RenderGraph graph;
	FakeTexture shared;
	FakeTexture readOutput;
	FakeTexture writeSource;
	auto sharedHandle = importTexture(graph, shared, "Shared");
	auto readOutputHandle = importTexture(graph, readOutput, "ReadOutput");
	auto writeSourceHandle = importTexture(graph, writeSource, "WriteSource");
	auto reader = graph.addBlitNode("Reader", sharedHandle, readOutputHandle);
	auto writer = graph.addBlitNode("Writer", writeSourceHandle, sharedHandle);
	auto root = addReadRoot(graph, readOutputHandle);
	root.dependsOn(reader);
	root.dependsOn(writer);
	std::string message;
	expect(!graph.compile(root, &message), "WAR hazard without a dependency path must fail compilation");
	expect(message.find("after reader") != std::string::npos, "WAR error should identify the prior reader");
}

void testMissingWawDependency()
{
	RenderGraph graph;
	FakeTexture firstSource;
	FakeTexture secondSource;
	FakeTexture shared;
	auto firstSourceHandle = importTexture(graph, firstSource, "FirstSource");
	auto secondSourceHandle = importTexture(graph, secondSource, "SecondSource");
	auto sharedHandle = importTexture(graph, shared, "Shared");
	auto firstWriter = graph.addBlitNode("FirstWriter", firstSourceHandle, sharedHandle);
	auto secondWriter = graph.addBlitNode("SecondWriter", secondSourceHandle, sharedHandle);
	auto root = addReadRoot(graph, sharedHandle);
	root.dependsOn(firstWriter);
	root.dependsOn(secondWriter);
	std::string message;
	expect(!graph.compile(root, &message), "WAW hazard without a dependency path must fail compilation");
	expect(message.find("after writer") != std::string::npos, "WAW error should identify the prior writer");
}

void testLegalExplicitDependencyChain()
{
	RenderGraph graph;
	FakeTexture source;
	FakeTexture shared;
	FakeTexture output;
	auto sourceHandle = importTexture(graph, source, "Source");
	auto sharedHandle = importTexture(graph, shared, "Shared");
	auto outputHandle = importTexture(graph, output, "Output");
	auto writer = graph.addBlitNode("Writer", sourceHandle, sharedHandle);
	auto reader = graph.addBlitNode("Reader", sharedHandle, outputHandle);
	reader.dependsOn(writer);
	std::string message;
	expect(graph.compile(reader, &message), "explicit RAW dependency chain should compile");
}

void testExecuteRequiresSuccessfulCompile()
{
	RenderGraph graph;
	FakeTexture source;
	FakeTexture destination;
	auto sourceHandle = importTexture(graph, source, "Source");
	auto destinationHandle = importTexture(graph, destination, "Destination");
	graph.addBlitNode("Blit", sourceHandle, destinationHandle);
	FakeRenderCommand command;
	RenderGraphContext context(&command, nullptr, nullptr);
	std::string message;
	expect(!graph.execute(context, &message), "execute must reject a graph that was not compiled successfully");
	expect(command.blits.empty(), "uncompiled graph must not execute passes in insertion order");
}

void testAttachmentIdentityAndConflict()
{
	RenderGraph graph;
	FakeTexture shared;
	FakeFrameBuffer firstFrameBuffer({&shared});
	FakeFrameBuffer secondFrameBuffer({&shared});
	auto firstDesc = textureDesc("FirstFrameBuffer");
	auto secondDesc = textureDesc("SecondFrameBuffer");
	auto first = graph.importFrameBuffer(firstDesc, &firstFrameBuffer);
	auto second = graph.importFrameBuffer(secondDesc, &secondFrameBuffer);
	auto firstColor = graph.colorAttachment(first);
	auto secondColor = graph.colorAttachment(second);
	expect(firstColor.isValid() && secondColor.isValid(), "framebuffer color attachments should be addressable");
	expect(firstColor.index() == secondColor.index(), "shared DeviceTexture pointer must reuse one resource identity");

	auto conflictDesc = textureDesc("ConflictingDepthImport");
	conflictDesc.role = TextureRoleEnum::AS_DEPTH;
	conflictDesc.format = ImageFormat::D16_UNorm;
	auto conflict = graph.importTexture(conflictDesc, &shared);
	expect(!conflict.isValid(), "conflicting import metadata must be rejected");
	auto root = addReadRoot(graph, firstColor);
	std::string message;
	expect(!graph.compile(root, &message), "a conflicting duplicate import must make compilation fail");
	expect(message.find("conflicting metadata") != std::string::npos, "compile error should retain the import conflict");
}

void testImportedFrameBufferRebind()
{
	RenderGraph graph;
	FakeTexture oldTexture;
	FakeTexture newTexture;
	FakeFrameBuffer oldFrameBuffer({&oldTexture}, nullptr, vec2(64.0f, 64.0f));
	FakeFrameBuffer newFrameBuffer({&newTexture}, nullptr, vec2(128.0f, 96.0f));
	auto frameBufferHandle = graph.importFrameBuffer(textureDesc("ImportedFrameBuffer"), &oldFrameBuffer);
	auto colorHandle = graph.colorAttachment(frameBufferHandle);
	expect(graph.rebindImportedFrameBuffer(frameBufferHandle, &newFrameBuffer, RenderGraphResourceLayout::ShaderRead),
		"imported framebuffer with the same attachment shape should rebind");
	expect(graph.colorAttachment(frameBufferHandle).index() == colorHandle.index(), "rebind must preserve attachment handles");
	expect(colorHandle->get<DeviceTexture>() == &newTexture, "rebind must update the attachment texture pointer");
	auto frameBufferResource = graph.resource(frameBufferHandle);
	auto colorResource = graph.resource(colorHandle);
	expect(frameBufferResource && frameBufferResource->desc.size.x == 128 && frameBufferResource->desc.size.y == 96,
		"rebind must update framebuffer dimensions");
	expect(colorResource && colorResource->desc.size.x == 128 && colorResource->desc.size.y == 96,
		"rebind must update attachment dimensions");
}

void testRejectedBarrierDoesNotAdvanceState()
{
	RenderGraph graph;
	FakeTexture source;
	FakeTexture destination;
	auto sourceHandle = importTexture(graph, source, "Source");
	auto destinationHandle = importTexture(graph, destination, "Destination");
	auto blit = graph.addBlitNode("Blit", sourceHandle, destinationHandle);
	std::string message;
	expect(graph.compile(blit, &message), "barrier rejection test graph should compile");
	auto initialState = RenderGraphTestAccess::colorState(graph, sourceHandle);
	FakeRenderCommand command;
	command.acceptBarriers = false;
	RenderGraphContext context(&command, nullptr, nullptr);
	expect(!graph.execute(context, &message), "backend barrier rejection must fail graph execution");
	auto finalState = RenderGraphTestAccess::colorState(graph, sourceHandle);
	expect(finalState.layout == initialState.layout && finalState.usage == initialState.usage,
		"logical resource state must not advance when the backend rejects a barrier");
}

void testSameLayoutWriteBarrier()
{
	RenderGraph graph;
	FakeTexture firstSource;
	FakeTexture secondSource;
	FakeTexture destination;
	auto firstSourceHandle = importTexture(graph, firstSource, "FirstSource");
	auto secondSourceHandle = importTexture(graph, secondSource, "SecondSource");
	auto destinationHandle = importTexture(graph, destination, "Destination");
	auto first = graph.addBlitNode("FirstWrite", firstSourceHandle, destinationHandle);
	auto second = graph.addBlitNode("SecondWrite", secondSourceHandle, destinationHandle);
	second.dependsOn(first);
	std::string message;
	expect(graph.compile(second, &message), "dependent consecutive writes should compile");

	FakeRenderCommand command;
	RenderGraphContext context(&command, nullptr, nullptr);
	expect(graph.execute(context, &message), "GPU-independent execution plan should succeed with a supporting backend command");
	bool foundSameLayoutWriteBarrier = false;
	for(const auto& barrier : command.barriers)
	{
		if(barrier.texture == &destination
			&& barrier.before.layout == DeviceTextureLayout::TransferDst
			&& barrier.after.layout == DeviceTextureLayout::TransferDst
			&& barrier.before.usage == DeviceTextureUsage::TransferWrite
			&& barrier.after.usage == DeviceTextureUsage::TransferWrite)
		{
			foundSameLayoutWriteBarrier = true;
			break;
		}
	}
	expect(foundSameLayoutWriteBarrier, "same-layout consecutive writes must emit a memory barrier");
}
void testConflictingAccessDeclarations()
{
	for(bool readWriteConflict : {false, true})
	{
		RenderGraph graph;
		FakeTexture source;
		FakeTexture destination;
		auto sourceHandle = importTexture(graph, source, "Source");
		auto destinationHandle = importTexture(graph, destination, "Destination");
		auto root = graph.addBlitNode("Blit", sourceHandle, destinationHandle);
		auto& pass = RenderGraphTestAccess::pass(graph, root);
		if(readWriteConflict)
		{
			pass.readWriteColor(destinationHandle);
		}
		else
		{
			pass.readStorageImage(sourceHandle);
		}
		std::string message;
		expect(!graph.compile(root, &message), "incompatible accesses within one pass must fail compilation");
		expect(message.find("conflicting accesses") != std::string::npos, "conflict diagnostic must include read-write and incompatible read-only accesses");
	}

	RenderGraph graph;
	FakeTexture source;
	auto sourceHandle = importTexture(graph, source, "Source");
	RenderGraphPassDesc desc;
	desc.name = "DuplicateRead";
	desc.readColor(sourceHandle).readColor(sourceHandle);
	expect(graph.compile(graph.addExternalNode(desc)), "identical read-only declarations should remain valid");
}

void testUnknownStateAndIncompatibleBeforeLayout()
{
	RenderGraph graph;
	FakeTexture source;
	auto desc = textureDesc("UnknownState");
	desc.initialColorLayout = RenderGraphResourceLayout::Unknown;
	auto sourceHandle = graph.importTexture(desc, &source);
	auto root = addReadRoot(graph, sourceHandle);
	std::string message;
	expect(!graph.compile(root, &message), "unknown initial layout must not be silently promoted to a known state");
	expect(message.find("unknown initial state") != std::string::npos, "unknown initial state must have a diagnostic");

	graph.setResourceLayout(sourceHandle, RenderGraphResourceLayout::ShaderRead);
	RenderGraphTestAccess::pass(graph, root).resourceAccesses[0].beforeLayout = RenderGraphResourceLayout::General;
	expect(!graph.compile(root, &message), "before layout contradicting the access type must be rejected");
	expect(message.find("incompatible before layout") != std::string::npos, "before layout mismatch must have a diagnostic");
}

void testExplicitReadFinalLayout()
{
	RenderGraph graph;
	FakeTexture source;
	auto sourceHandle = importTexture(graph, source, "Source");
	auto root = addReadRoot(graph, sourceHandle);
	RenderGraphTestAccess::pass(graph, root).resourceAccesses[0].afterLayout = RenderGraphResourceLayout::TransferSrc;
	expect(graph.compile(root), "read with explicit final layout should compile");
	FakeRenderCommand command;
	RenderGraphContext context(&command, nullptr, nullptr);
	expect(graph.execute(context), "read with explicit final layout should execute");
	expect(command.barriers.size() == 1 && command.barriers.back().after.layout == DeviceTextureLayout::TransferSrc,
		"read access must honor an explicitly requested final layout");
}

void testWriteWithoutExplicitFinalLayout()
{
	RenderGraph graph;
	FakeTexture source;
	FakeTexture destination;
	auto sourceHandle = importTexture(graph, source, "Source");
	auto destinationHandle = importTexture(graph, destination, "Destination");
	RenderGraphTestAccess::markGraphOwnedAndUninitialized(graph, destinationHandle);
	auto root = graph.addBlitNode("Blit", sourceHandle, destinationHandle);
	RenderGraphTestAccess::pass(graph, root).resourceAccesses[1].afterLayout = RenderGraphResourceLayout::Unknown;
	expect(graph.compile(root), "write without an explicit final layout should compile");
	FakeRenderCommand command;
	RenderGraphContext context(&command, nullptr, nullptr);
	expect(graph.execute(context), "write without an explicit final layout should execute");
	expect(graph.resource(destinationHandle)->contentsInitialized, "successful writes must initialize contents even without a final transition");
	expect(RenderGraphTestAccess::colorState(graph, destinationHandle).layout == DeviceTextureLayout::TransferDst,
		"omitted final layout must retain the access layout");
}

void testRebindRejectsSharedAndDuplicateAttachments()
{
	RenderGraph graph;
	FakeTexture shared;
	FakeTexture replacement;
	FakeFrameBuffer first({&shared});
	FakeFrameBuffer second({&shared});
	FakeFrameBuffer rebound({&replacement});
	auto firstHandle = graph.importFrameBuffer(textureDesc("First"), &first);
	auto secondHandle = graph.importFrameBuffer(textureDesc("Second"), &second);
	auto color = graph.colorAttachment(firstHandle);
	auto root = addReadRoot(graph, color);
	expect(graph.compile(root), "graph using shared imported attachments should compile");
	expect(!graph.rebindImportedFrameBuffer(firstHandle, &rebound, RenderGraphResourceLayout::ShaderRead),
		"rebind must reject replacing an attachment shared by another framebuffer");
	expect(graph.colorAttachment(secondHandle)->get<DeviceTexture>() == &shared && firstHandle->get<DeviceFrameBuffer>() == &first,
		"failed shared rebind must leave all original bindings intact");
	FakeRenderCommand command;
	RenderGraphContext context(&command, nullptr, nullptr);
	expect(graph.execute(context), "failed rebind must preserve the compiled graph");

	FakeTexture other;
	FakeFrameBuffer multiple({&shared, &other});
	FakeFrameBuffer duplicates({&replacement, &replacement});
	auto multipleHandle = graph.importFrameBuffer(textureDesc("Multiple"), &multiple);
	expect(!graph.rebindImportedFrameBuffer(multipleHandle, &duplicates, RenderGraphResourceLayout::ShaderRead),
		"rebind must reject duplicate new texture pointers before mutating resource identities");
	expect(multipleHandle->get<DeviceFrameBuffer>() == &multiple, "duplicate attachment rejection must preserve the framebuffer");
}

void testDepthOnlyRebindAndUpdatedMetadata()
{
	RenderGraph graph;
	FakeTexture oldDepth;
	FakeTexture newDepth;
	FakeFrameBuffer original({}, &oldDepth);
	FakeFrameBuffer replacement({}, &newDepth);
	auto desc = textureDesc("DepthOnly");
	desc.initialDepthLayout = RenderGraphResourceLayout::DepthAttachment;
	auto target = graph.importFrameBuffer(desc, &original);
	expect(graph.rebindImportedFrameBuffer(target, &replacement, RenderGraphResourceLayout::Unknown, RenderGraphResourceLayout::DepthRead),
		"depth-only framebuffer rebind should not require a color state");
	auto depthHandle = graph.depthAttachment(target);
	auto attachmentDesc = graph.resource(depthHandle)->desc;
	expect(attachmentDesc.initialDepthLayout == RenderGraphResourceLayout::DepthRead, "rebind must update attachment initial state metadata");
	auto importedAgain = graph.importTexture(attachmentDesc, &newDepth);
	expect(importedAgain.isValid() && importedAgain.index() == depthHandle.index(), "reimport after rebind must preserve identity without a false metadata conflict");
}

void testBlitFitsBothTextures()
{
	RenderGraph graph;
	FakeTexture source;
	FakeTexture destination;
	auto sourceDesc = textureDesc("WideSource");
	sourceDesc.size = vec2(128.0f, 32.0f);
	auto destinationDesc = textureDesc("TallDestination");
	destinationDesc.size = vec2(64.0f, 96.0f);
	auto sourceHandle = graph.importTexture(sourceDesc, &source);
	auto destinationHandle = graph.importTexture(destinationDesc, &destination);
	auto root = graph.addBlitNode("Blit", sourceHandle, destinationHandle);
	expect(graph.compile(root), "blit between differently sized resources should compile");
	FakeRenderCommand command;
	RenderGraphContext context(&command, nullptr, nullptr);
	expect(graph.execute(context), "blit between differently sized resources should execute");
	expect(command.blits.size() == 1 && command.blits[0].size.x == 64 && command.blits[0].size.y == 32,
		"blit extent must fit within both source and destination");
}

void testImplicitRasterFinalLayouts()
{
	RenderGraph graph;
	FakeTexture color;
	FakeTexture depth;
	FakeFrameBuffer frameBuffer({&color}, &depth);
	auto desc = textureDesc("RasterTarget");
	desc.initialColorLayout = RenderGraphResourceLayout::ColorAttachment;
	auto target = graph.importFrameBuffer(desc, &frameBuffer);
	FakeRenderPass renderPass;
	renderPass.init({{ImageFormat::RGBA8_UNorm, false}, {ImageFormat::D24_UNorm_S8_UInt, true}}, DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, true);
	RenderGraphPassDesc accesses;
	accesses.writeColor(graph.colorAttachment(target), RenderGraphResourceLayout::ShaderRead)
		.writeDepth(graph.depthAttachment(target), RenderGraphResourceLayout::DepthRead);
	FakeRenderCommand command;
	RenderGraphContext context(&command, nullptr, nullptr);
	std::string message;
	expect(RenderGraphTestAccess::finishRasterPass(graph, context, target, &renderPass, accesses.resourceAccesses, message),
		"final state tracking should accept implicit render pass transitions");
	expect(command.barriers.size() == 2, "both color and depth attachment writes need visibility to shader reads");
	for(const auto& barrier : command.barriers)
	{
		auto expectedLayout = barrier.texture == &color ? DeviceTextureLayout::ShaderRead : DeviceTextureLayout::DepthRead;
		expect(barrier.before.layout == expectedLayout && barrier.after.layout == expectedLayout,
			"post-pass barriers must start from the backend's actual final layout");
		expect(barrier.before.usage == (barrier.texture == &color ? DeviceTextureUsage::ColorAttachmentWrite : DeviceTextureUsage::DepthAttachmentWrite),
			"implicit layout tracking must preserve the producer write scope");
	}
}

RenderGraphIndexedDrawData indexedTriangle(RenderGraphResourceHandle texture, bool index32 = false)
{
	RenderGraphIndexedDrawData data;
	data.vertexLayout = {8, {{VertexAttributeFormat::Float2, 0}}};
	data.vertices.resize(4 * 8);
	data.uniformData.resize(64);
	data.indexType = index32 ? RenderGraphIndexType::UInt32 : RenderGraphIndexType::UInt16;
	const uint32_t wideIndices[] = {0, 0, 1, 2};
	const uint16_t shortIndices[] = {0, 0, 1, 2};
	data.indices.resize(index32 ? sizeof(wideIndices) : sizeof(shortIndices));
	std::memcpy(data.indices.data(), index32 ? static_cast<const void*>(wideIndices) : shortIndices, data.indices.size());
	RenderGraphIndexedDraw draw;
	draw.texture = texture;
	draw.indexCount = 3;
	draw.firstIndex = 1;
	draw.vertexOffset = 1;
	draw.scissor = vec4(-5, -10, 100, 100);
	data.draws.push_back(draw);
	return data;
}

void testIndexedDrawValidation()
{
	RenderGraph graph;
	FakeTexture texture;
	auto handle = importTexture(graph, texture, "IndexedTexture");
	for(bool wide : {false, true})
	{
		auto data = indexedTriangle(handle, wide);
		expect(data.validate(), "both index widths must accept base vertex and first index offsets");
		data.draws[0].vertexOffset = 2;
		expect(!data.validate(), "base vertex must not overrun uploaded vertices");
		data.draws[0].vertexOffset = -1;
		expect(!data.validate(), "negative resolved vertex index must fail");
		data = indexedTriangle(handle, wide);
		data.draws[0].indexCount = 4;
		expect(!data.validate(), "out-of-range index count must fail");
		data = indexedTriangle(handle, wide);
		data.indices.pop_back();
		expect(!data.validate(), "partial index element must fail");
	}
	auto data = indexedTriangle(handle);
	data.vertexLayout.attributes[0].offset = 4;
	expect(!data.validate(), "attribute cannot extend past vertex stride");
	data = indexedTriangle(handle);
	data.draws[0].scissor.x = std::numeric_limits<float>::infinity();
	expect(!data.validate(), "non-finite scissor must fail");
	data = indexedTriangle(handle);
	data.uniformData.clear();
	expect(!data.validate(), "indexed draws must have uniform data");
	data = indexedTriangle(handle);
	data.draws.clear();
	data.vertices.clear();
	data.indices.clear();
	expect(data.validate(), "empty ImGui frames are valid");
}

void testMultipleViewQueuesAndCameras()
{
	FakeBackend backend;
	RenderGraph graph;
	FakeTexture shadowDepth, mainColor, mirrorColor;
	FakeFrameBuffer shadowTarget({}, &shadowDepth), mainTarget({&mainColor}), mirrorTarget({&mirrorColor}, nullptr, vec2(32, 16));
	auto shadowDesc = textureDesc("Shadow");
	shadowDesc.role = TextureRoleEnum::AS_DEPTH;
	auto shadow = graph.importFrameBuffer(shadowDesc, &shadowTarget);
	auto main = graph.importFrameBuffer(textureDesc("Main"), &mainTarget);
	auto mirror = graph.importFrameBuffer(textureDesc("Mirror"), &mirrorTarget);
	RenderQueue shadowQueue, mainQueue, mirrorQueue;
	Camera mainCamera, mirrorCamera;
	RenderGraphRasterPassDesc pass;
	pass.name = "Shadow";
	pass.frameBufferResource = shadow;
	pass.attachments = {{ImageFormat::D24_UNorm_S8_UInt, true}};
	pass.opType = DeviceRenderPass::OpType::LOADCLEAR_AND_STORE;
	pass.sceneQueue = &shadowQueue;
	RenderGraphPassDesc accesses;
	accesses.writeDepth(graph.depthAttachment(shadow), RenderGraphResourceLayout::DepthRead);
	pass.resourceAccesses = accesses.resourceAccesses;
	auto shadowNode = graph.addRasterNode(pass, [](RenderGraphPassContext& context) { context.drawSceneQueue(); });
	expect(shadowNode.output().index() == graph.depthAttachment(shadow).index(), "depth-only views should expose depth as default output");
	auto addView = [&](RenderGraphResourceHandle target, Camera* camera, RenderQueue* queue)
	{
		pass.name = "Scene";
		pass.frameBufferResource = target;
		pass.attachments = {{ImageFormat::RGBA8_UNorm, false}};
		pass.sceneQueue = queue;
		pass.camera = camera;
		accesses.resourceAccesses.clear();
		accesses.readDepth(shadowNode.output()).writeColor(graph.colorAttachment(target), RenderGraphResourceLayout::ShaderRead);
		pass.resourceAccesses = accesses.resourceAccesses;
		return graph.addRasterNode(pass, [camera, queue](RenderGraphPassContext& context)
		{
			expect(context.camera() == camera && context.sceneQueue() == queue, "view context must remain local to each pass");
			context.drawSceneQueue();
		}).dependsOn(shadowNode);
	};
	auto mainNode = addView(main, &mainCamera, &mainQueue);
	auto mirrorNode = addView(mirror, &mirrorCamera, &mirrorQueue);
	RenderGraphPassDesc rootDesc;
	rootDesc.name = "CompositeViews";
	rootDesc.readColor(mainNode.output()).readColor(mirrorNode.output());
	auto root = graph.addExternalNode(rootDesc).dependsOn(mainNode).dependsOn(mirrorNode);
	std::string message;
	expect(graph.compile(root, &message), "independent views sharing a shadow producer must compile: " + message);
	FakeRenderCommand command;
	RenderQueue fallbackQueue;
	RenderGraphContext context(&command, nullptr, &fallbackQueue);
	expect(graph.execute(context, &message), "multi-view graph must execute: " + message);
	expect(backend.stages.size() == 3, "different view targets must have distinct stages even with the same pass name");
	expect(backend.stages[0]->lastQueue == &shadowQueue && backend.stages[1]->lastQueue == &mainQueue
		&& backend.stages[2]->lastQueue == &mirrorQueue, "pass queues must override the legacy frame queue");
	expect(backend.stages[1]->observedCamera == &mainCamera && backend.stages[2]->observedCamera == &mirrorCamera,
		"backend uniform updates must receive the correct view camera");
}

void testIndexedOverlayAndPresent()
{
	for(bool wide : {false, true})
	{
		FakeBackend backend;
		RenderGraph graph;
		FakeTexture screenColor, sampledTexture;
		FakeFrameBuffer screen({&screenColor});
		backend.swapchain = &screen;
		auto target = graph.importSwapChainFrameBuffer(0);
		auto color = graph.colorAttachment(target);
		auto sampled = importTexture(graph, sampledTexture, "OffscreenView");
		expect(graph.importSampledTexture("ImGuiAlias", &sampledTexture).index() == sampled.index(),
			"ImGui imports must reuse the offscreen view resource identity");
		RenderGraphRasterPassDesc pass;
		pass.name = "ScreenComposite";
		pass.frameBufferResource = target;
		pass.attachments = {{ImageFormat::RGBA8_UNorm, false}};
		pass.opType = DeviceRenderPass::OpType::LOADCLEAR_AND_STORE;
		RenderGraphPassDesc accesses;
		accesses.writeColor(color);
		pass.resourceAccesses = accesses.resourceAccesses;
		auto composite = graph.addRasterNode(pass);
		pass.name = "GuiOverlay";
		pass.opType = DeviceRenderPass::OpType::LOAD_AND_STORE;
		accesses.resourceAccesses.clear();
		accesses.readWriteColor(color);
		pass.resourceAccesses = accesses.resourceAccesses;
		auto gui = graph.addRasterNode(pass).dependsOn(composite);
		pass.name = "ImGuiOverlay";
		MaterialInstance material;
		pass.material = &material;
		auto data = indexedTriangle(sampled, wide);
		auto clipped = data.draws[0];
		clipped.scissor = vec4(100, 100, 5, 5);
		data.draws.insert(data.draws.begin(), clipped);
		int callbacks = 0;
		RenderGraphIndexedDraw callback;
		callback.callback = [&callbacks]() { callbacks++; };
		data.draws.insert(data.draws.begin(), callback);
		auto overlay = graph.addIndexedRasterNode(pass, std::move(data)).dependsOn(gui);
		auto present = graph.addPresentNode("Present", color).dependsOn(overlay);
		std::string message;
		expect(graph.compile(present, &message), "screen, GUI, indexed overlay, present chain must compile: " + message);
		FakeRenderCommand command;
		RenderGraphContext context(&command, nullptr, nullptr);
		expect(graph.execute(context, &message), "indexed overlay must execute: " + message);
		auto stage = backend.stages.back();
		expect(callbacks == 1 && stage->drawCount == 1, "callbacks execute once and fully clipped geometry is skipped");
		expect(stage->index32 == wide && stage->lastFirstIndex == 1 && stage->lastVertexOffset == 1,
			"index width and draw offsets must reach the backend unchanged");
		expect(stage->scissor.x == 0 && stage->scissor.y == 0 && stage->scissor.z == 64 && stage->scissor.w == 64,
			"scissor must be clamped to target extent");
		expect(!command.barriers.empty() && command.barriers.back().texture == &screenColor
			&& command.barriers.back().before.layout == DeviceTextureLayout::ColorAttachment
			&& command.barriers.back().after.layout == DeviceTextureLayout::Present,
			"only final graph node transitions the swapchain to presentation");
		expect(RenderGraphTestAccess::colorState(graph, color).layout == DeviceTextureLayout::Present,
			"graph must preserve presentation state for the next frame");
	}
}

void testPresentRejectsOffscreenTexture()
{
	RenderGraph graph;
	FakeTexture texture;
	auto resource = importTexture(graph, texture, "Offscreen");
	auto present = graph.addPresentNode("InvalidPresent", resource);
	std::string message;
	expect(!graph.compile(present, &message) && message.find("not a swapchain") != std::string::npos,
		"Present must reject textures outside the swapchain");
}

}
}

int main()
{
	using namespace tzw;
	Engine::shared()->setRenderBackEnd(nullptr);
	testTypedResourceHandles();
	testOwnedEmptyTexture();
	testNeutralStageAndUniformUpload();
	testBasicImageFormats();
	testDependencyCycle();
	testInvalidResourceHandle();
	testGraphOwnedReadBeforeWrite();
	testMissingRawDependency();
	testMissingWarDependency();
	testMissingWawDependency();
	testLegalExplicitDependencyChain();
	testExecuteRequiresSuccessfulCompile();
	testAttachmentIdentityAndConflict();
	testImportedFrameBufferRebind();
	testRejectedBarrierDoesNotAdvanceState();
	testSameLayoutWriteBarrier();
	testConflictingAccessDeclarations();
	testUnknownStateAndIncompatibleBeforeLayout();
	testExplicitReadFinalLayout();
	testWriteWithoutExplicitFinalLayout();
	testRebindRejectsSharedAndDuplicateAttachments();
	testDepthOnlyRebindAndUpdatedMetadata();
	testImplicitRasterFinalLayouts();
	testBlitFitsBothTextures();
	testIndexedDrawValidation();
	testMultipleViewQueuesAndCameras();
	testIndexedOverlayAndPresent();
	testPresentRejectsOffscreenTexture();
	if(failureCount != 0)
	{
		std::cerr << failureCount << " RenderGraph test(s) failed.\n";
		return 1;
	}
	std::cout << "All RenderGraph tests passed.\n";
	return 0;
}
