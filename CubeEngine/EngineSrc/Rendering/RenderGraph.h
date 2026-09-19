#pragma once

#include <cstdint>
#include <cstddef>
#include <type_traits>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "BackEnd/DeviceRenderPass.h"
#include "BackEnd/DeviceRenderCommand.h"
#include "BackEnd/DeviceTexture.h"
#include "Math/vec2.h"
#include "Rendering/DrawPass.h"
#include "Rendering/VertexLayout.h"
#include "Math/vec4.h"
#include "Technique/MaterialTechnique.h"

namespace tzw
{
class Camera;
class DeviceBuffer;
class DeviceDescriptor;
class DeviceFrameBuffer;
class DeviceMaterial;
class DevicePipeline;
class DeviceRenderCommand;
class DeviceRenderStage;
class DeviceShaderCollection;
class MaterialInstance;
class RenderGraph;
class RenderGraphContext;
class RenderPath;
class RenderQueue;
struct RenderGraphPassDesc;
struct RenderGraphResource;

using RenderGraphResourceLayout = DeviceTextureLayout;

enum class RenderGraphResourceAccessType
{
	ReadColor,
	ReadDepth,
	WriteColor,
	WriteDepth,
	ReadWriteColor,
	ReadWriteDepth,
	TransferRead,
	TransferWrite,
	ReadStorageImage,
	WriteStorageImage,
	ReadWriteStorageImage,
	Present,
};

using RenderGraphResourceUsage = DeviceTextureUsage;
using RenderGraphResourceState = DeviceTextureState;

enum class RenderGraphResourceKind
{
	Invalid,
	Texture,
	FrameBuffer,
};

class RenderGraphResourceHandle
{
public:
	RenderGraphResourceHandle();

	bool isValid() const;
	uint32_t index() const;
	static RenderGraphResourceHandle invalid();
	// The graph must outlive its handles; invalid/stale handles resolve to an Invalid resource.
	const RenderGraphResource* operator->() const;

private:
	friend class RenderGraph;

	RenderGraphResourceHandle(const RenderGraph* graph, uint32_t index, uint64_t generation);

	uint32_t m_index;
	const RenderGraph* m_graph = nullptr;
	uint64_t m_generation = 0;
};

class RenderGraphPassHandle
{
public:
	RenderGraphPassHandle();

	bool isValid() const;
	uint32_t index() const;
	static RenderGraphPassHandle invalid();

private:
	friend class RenderGraph;

	explicit RenderGraphPassHandle(uint32_t index);

	uint32_t m_index;
};

class RenderGraphNode
{
public:
	RenderGraphNode();

	bool isValid() const;
	RenderGraphPassHandle passHandle() const;
	RenderGraphResourceHandle output() const;
	RenderGraphResourceHandle output(const std::string& name) const;
	RenderGraphNode withOutput(RenderGraphResourceHandle resource) const;
	RenderGraphNode withOutput(const std::string& name, RenderGraphResourceHandle resource) const;
	RenderGraphNode connect(RenderGraphNode next) const;
	RenderGraphNode dependsOn(RenderGraphNode dependency) const;

private:
	friend class RenderGraph;

	RenderGraphNode(RenderGraph* graph, RenderGraphPassHandle passHandle, RenderGraphResourceHandle output);

	RenderGraph* m_graph;
	RenderGraphPassHandle m_passHandle;
	RenderGraphResourceHandle m_output;
};

struct RenderGraphResourceDesc
{
	std::string name;
	ImageFormat format = ImageFormat::Surface;
	TextureRoleEnum role = TextureRoleEnum::AS_COLOR;
	TextureUsageEnum usage = TextureUsageEnum::SAMPLE_AND_ATTACHMENT;
	vec2 size = vec2(0, 0);
	bool imported = true;
	RenderGraphResourceLayout initialColorLayout = RenderGraphResourceLayout::ColorAttachment;
	RenderGraphResourceLayout initialDepthLayout = RenderGraphResourceLayout::DepthAttachment;
};

struct RenderGraphResourceAccess
{
	RenderGraphResourceHandle resource;
	RenderGraphResourceAccessType type = RenderGraphResourceAccessType::ReadColor;
	RenderGraphResourceLayout beforeLayout = RenderGraphResourceLayout::Unknown;
	RenderGraphResourceLayout afterLayout = RenderGraphResourceLayout::Unknown;
};

enum class RenderGraphPassKind
{
	Raster,
	Compute,
	Blit,
	Present,
	External,
};

struct RenderGraphResource
{
    RenderGraphResourceKind type() const { return kind; }

    template<typename T>
    T* get() const
    {
        if constexpr(std::is_same_v<T, DeviceTexture>)
        {
            return kind == RenderGraphResourceKind::Texture ? texture : nullptr;
        }
        else if constexpr(std::is_same_v<T, DeviceFrameBuffer>)
        {
            return kind == RenderGraphResourceKind::FrameBuffer ? frameBuffer : nullptr;
        }
        else
        {
            static_assert(std::is_same_v<T, DeviceTexture> || std::is_same_v<T, DeviceFrameBuffer>,
                "Unsupported RenderGraph resource type");
        }
    }

	RenderGraphResourceDesc desc;
	RenderGraphResourceKind kind = RenderGraphResourceKind::Invalid;
	DeviceTexture* texture = nullptr;
	DeviceTexture* depthTexture = nullptr;
	DeviceFrameBuffer* frameBuffer = nullptr;
	DeviceRenderPass* renderPass = nullptr;
	bool ownsTexture = false;
	bool isSwapChainImage = false;
	bool ownsFrameBuffer = false;
	bool ownsRenderPass = false;
	std::vector<RenderGraphResourceHandle> colorAttachments;
	RenderGraphResourceHandle depthAttachment = RenderGraphResourceHandle::invalid();
	bool contentsInitialized = false;
	RenderGraphResourceState currentColorState;
	RenderGraphResourceState currentDepthState;
};

enum class RenderGraphIndexType
{
	UInt16,
	UInt32,
};

struct RenderGraphIndexedDraw
{
	RenderGraphResourceHandle texture;
	uint32_t indexCount = 0;
	uint32_t firstIndex = 0;
	int32_t vertexOffset = 0;
	vec4 scissor;
	std::function<void()> callback;
};

struct RenderGraphIndexedDrawData
{
	VertexLayout vertexLayout;
	RenderGraphIndexType indexType = RenderGraphIndexType::UInt16;
	std::vector<uint8_t> vertices;
	std::vector<uint8_t> indices;
	std::vector<uint8_t> uniformData;
	uint32_t uniformBinding = 0;
	uint32_t textureBinding = 1;
	std::vector<RenderGraphIndexedDraw> draws;

	bool validate(std::string* outMessage = nullptr) const;
};

struct RenderGraphRasterPassDesc
{
	std::string name;
	DeviceAttachmentInfoList attachments;
	DeviceRenderPass::OpType opType = DeviceRenderPass::OpType::LOAD_AND_STORE;
	bool isNeedTransitionToRead = false;
	bool isOutputToScreen = false;
	RenderGraphResourceDesc frameBufferDesc;
	RenderGraphResourceHandle frameBufferResource = RenderGraphResourceHandle::invalid();
	DrawPassTypeMask drawPassMask = DrawPassType::Unset;
	bool consumesSceneQueue = false;
	MaterialInstance* material = nullptr;
	RenderQueue* sceneQueue = nullptr;
	Camera* camera = nullptr;
	VertexLayout vertexLayout;
	bool dynamicScissor = false;
	std::vector<RenderGraphResourceAccess> resourceAccesses;
};

struct RenderGraphComputePassDesc
{
	std::string name;
	DeviceShaderCollection* shaderCollection = nullptr;
	std::vector<RenderGraphResourceAccess> resourceAccesses;
};

class RenderGraphPassContext
{
public:
	DeviceRenderCommand* cmd() const;
	RenderPath* renderPath() const;
	RenderQueue* sceneQueue() const;
	Camera* camera() const;
	const RenderGraphPassDesc* pass() const;
	const RenderGraphResource* resource(RenderGraphResourceHandle handle) const;
	DeviceFrameBuffer* targetFrameBuffer() const;
	DeviceMaterial* material() const;
	DeviceDescriptor* materialDescriptor() const;
	DevicePipeline* pipeline() const;
	DeviceDescriptor* itemDescriptor() const;
	void bindSinglePipelineDescriptor();
	void bindSinglePipelineDescriptor(DeviceDescriptor* extraItemDescriptor);
	void bindSinglePipelineDescriptorCompute();
	void drawQueue(RenderQueue* renderQueue, MaterialTechniqueType techniqueType = MaterialTechniqueType::Default);
	void drawSceneQueue(MaterialTechniqueType techniqueType = MaterialTechniqueType::Default);
	bool bindItemUniform(uint32_t binding, const void* data, size_t size);
	void bindTexture(uint32_t binding, RenderGraphResourceHandle resource);
	void bindTextures(uint32_t binding, const std::vector<RenderGraphResourceHandle>& resources);
	void drawScreenQuad();
	void drawSphere();
	void dispatch(uint32_t x, uint32_t y, uint32_t z);

private:
	friend class RenderGraph;

	RenderGraphPassContext(RenderGraphContext* graphContext, RenderGraphPassDesc* pass);

	DeviceRenderStage* stage() const;

	RenderGraphContext* m_graphContext;
	RenderGraphPassDesc* m_pass;
};

class RenderGraphContext
{
public:
	RenderGraphContext(DeviceRenderCommand* cmd, RenderPath* renderPath, RenderQueue* sceneQueue);

	DeviceRenderCommand* cmd() const;
	RenderPath* renderPath() const;
	RenderQueue* sceneQueue() const;
	const RenderGraphPassDesc* pass() const;
	const RenderGraphResource* resource(RenderGraphResourceHandle handle) const;

private:
	friend class RenderGraph;

	void setGraph(const RenderGraph* graph);
	void setPass(const RenderGraphPassDesc* pass);

	DeviceRenderCommand* m_cmd;
	RenderPath* m_renderPath;
	RenderQueue* m_sceneQueue;
	const RenderGraph* m_graph;
	const RenderGraphPassDesc* m_pass;
};

struct RenderGraphPassDesc
{
	RenderGraphPassDesc& readColor(RenderGraphResourceHandle resource);
	RenderGraphPassDesc& readDepth(RenderGraphResourceHandle resource);
	RenderGraphPassDesc& writeColor(RenderGraphResourceHandle resource, RenderGraphResourceLayout finalLayout = RenderGraphResourceLayout::ColorAttachment);
	RenderGraphPassDesc& writeDepth(RenderGraphResourceHandle resource, RenderGraphResourceLayout finalLayout = RenderGraphResourceLayout::DepthAttachment);
	RenderGraphPassDesc& readWriteColor(RenderGraphResourceHandle resource, RenderGraphResourceLayout finalLayout = RenderGraphResourceLayout::ColorAttachment);
	RenderGraphPassDesc& readWriteDepth(RenderGraphResourceHandle resource, RenderGraphResourceLayout finalLayout = RenderGraphResourceLayout::DepthAttachment);
	RenderGraphPassDesc& transferRead(RenderGraphResourceHandle resource);
	RenderGraphPassDesc& transferWrite(RenderGraphResourceHandle resource);
	RenderGraphPassDesc& readStorageImage(RenderGraphResourceHandle resource);
	RenderGraphPassDesc& writeStorageImage(RenderGraphResourceHandle resource, RenderGraphResourceLayout finalLayout = RenderGraphResourceLayout::General);
	RenderGraphPassDesc& readWriteStorageImage(RenderGraphResourceHandle resource, RenderGraphResourceLayout finalLayout = RenderGraphResourceLayout::General);

	std::string name;
	DrawPassTypeMask consumedDrawPassMask = DrawPassType::Unset;
	std::vector<RenderGraphResourceAccess> resourceAccesses;
	std::function<void(RenderGraphPassContext&)> execute;

private:
	friend class RenderGraph;
	friend class RenderGraphPassContext;
	friend class RenderGraphTestAccess;

	struct NamedOutput
	{
		std::string name;
		RenderGraphResourceHandle resource;
	};

	RenderGraphPassKind kind = RenderGraphPassKind::External;
	RenderGraphPassHandle handle = RenderGraphPassHandle::invalid();
	DeviceRenderPass* renderPass = nullptr;
	DeviceRenderStage* compiledStage = nullptr;
	RenderQueue* sceneQueue = nullptr;
	Camera* camera = nullptr;
	std::shared_ptr<const RenderGraphIndexedDrawData> indexedDrawData;
	DeviceBuffer* vertexBuffer = nullptr;
	DeviceBuffer* indexBuffer = nullptr;
	DeviceBuffer* uniformBuffer = nullptr;
	DeviceRenderPass::OpType opType = DeviceRenderPass::OpType::LOAD_AND_STORE;
	RenderGraphResourceHandle frameBufferResource = RenderGraphResourceHandle::invalid();
	RenderGraphResourceHandle blitSource = RenderGraphResourceHandle::invalid();
	RenderGraphResourceHandle blitDestination = RenderGraphResourceHandle::invalid();
	RenderGraphResourceHandle outputResource = RenderGraphResourceHandle::invalid();
	bool isOutputToScreen = false;
	std::vector<NamedOutput> namedOutputs;
	std::vector<RenderGraphPassHandle> dependencies;
};

class RenderGraph
{
public:
	RenderGraph();
	~RenderGraph();
	RenderGraph(const RenderGraph&) = delete;
	RenderGraph& operator=(const RenderGraph&) = delete;
	RenderGraph(RenderGraph&&) = delete;
	RenderGraph& operator=(RenderGraph&&) = delete;

	void beginBuild();
	void clear();
	void clearResources();
	RenderGraphPassHandle addPass(const RenderGraphPassDesc& desc);
	RenderGraphNode addRasterNode(const RenderGraphRasterPassDesc& desc, std::function<void(RenderGraphPassContext&)> execute = nullptr);
	RenderGraphNode addFullscreenNode(const RenderGraphRasterPassDesc& desc, std::function<void(RenderGraphPassContext&)> execute = nullptr);
	RenderGraphNode addComputeNode(const RenderGraphComputePassDesc& desc, std::function<void(RenderGraphPassContext&)> execute = nullptr);
	RenderGraphNode addBlitNode(const std::string& name, RenderGraphResourceHandle source, RenderGraphResourceHandle destination);
	RenderGraphNode addExternalNode(const RenderGraphPassDesc& desc);
	RenderGraphNode addIndexedRasterNode(const RenderGraphRasterPassDesc& desc, RenderGraphIndexedDrawData data);
	RenderGraphNode addPresentNode(const std::string& name, RenderGraphResourceHandle resource);
	RenderGraphResourceHandle importSwapChainFrameBuffer(uint32_t imageIndex);
	RenderGraphResourceHandle importSampledTexture(const std::string& name, DeviceTexture* texture);
	RenderGraphResourceHandle createTexture(const RenderGraphResourceDesc& desc);
	RenderGraphResourceHandle createTexture(const RenderGraphResourceDesc& desc, const unsigned char* pixels);
	// Borrowed until clearResources(); compiled stages are released before these shaders.
	DeviceShaderCollection* createComputeShader(const std::string& path);
	RenderGraphPassHandle addRasterPass(const RenderGraphRasterPassDesc& desc, std::function<void(RenderGraphPassContext&)> execute = nullptr);
	RenderGraphPassHandle addFullscreenPass(const RenderGraphRasterPassDesc& desc, std::function<void(RenderGraphPassContext&)> execute = nullptr);
	RenderGraphPassHandle addComputePass(const RenderGraphComputePassDesc& desc, std::function<void(RenderGraphPassContext&)> execute = nullptr);
	RenderGraphPassHandle addBlitPass(const std::string& name, RenderGraphResourceHandle source, RenderGraphResourceHandle destination);
	RenderGraphPassHandle addExternalPass(const RenderGraphPassDesc& desc);
	RenderGraphResourceHandle importTexture(const RenderGraphResourceDesc& desc, DeviceTexture* texture);
	RenderGraphResourceHandle importFrameBuffer(const RenderGraphResourceDesc& desc, DeviceFrameBuffer* frameBuffer);
	RenderGraphResourceHandle createFrameBuffer(const RenderGraphResourceDesc& desc, DeviceRenderPass* renderPass);
	RenderGraphResourceHandle createFrameBuffer(const RenderGraphResourceDesc& desc, const DeviceAttachmentInfoList& attachments,
		DeviceRenderPass::OpType opType, bool isNeedTransitionToRead, bool isOutputToScreen = false);
	RenderGraphResourceHandle colorAttachment(RenderGraphResourceHandle frameBufferResource, uint32_t index = 0) const;
	RenderGraphResourceHandle depthAttachment(RenderGraphResourceHandle frameBufferResource) const;
	bool rebindImportedFrameBuffer(RenderGraphResourceHandle handle, DeviceFrameBuffer* frameBuffer,
		RenderGraphResourceLayout initialColorLayout, RenderGraphResourceLayout initialDepthLayout = RenderGraphResourceLayout::Unknown);
	void resizeOwnedFrameBuffers(vec2 size);
	void setResourceLayout(RenderGraphResourceHandle handle, RenderGraphResourceLayout colorLayout, RenderGraphResourceLayout depthLayout = RenderGraphResourceLayout::Unknown);
	DeviceFrameBuffer* passFrameBuffer(RenderGraphPassHandle handle) const;
	RenderGraphResource* resource(RenderGraphResourceHandle handle);
	const RenderGraphResource* resource(RenderGraphResourceHandle handle) const;
	bool validate(std::string* outMessage = nullptr) const;
	std::string dump() const;
	std::string dump(RenderGraphNode root) const;
	std::string dumpCompiledOrder() const;
	std::string dumpDot() const;
	std::string dumpDot(RenderGraphNode root) const;
	bool compile(RenderGraphNode root, std::string* outMessage = nullptr);
	bool execute(RenderGraphContext& context, std::string* outMessage = nullptr);

private:
	friend class RenderGraphNode;
	friend class RenderGraphTestAccess;

	struct RasterPassCacheEntry
	{
		std::string key;
		DeviceRenderPass* renderPass = nullptr;
		DeviceRenderStage* stage = nullptr;
		DeviceBuffer* vertexBuffer = nullptr;
		DeviceBuffer* indexBuffer = nullptr;
		DeviceBuffer* uniformBuffer = nullptr;
		RenderGraphResourceHandle frameBufferResource = RenderGraphResourceHandle::invalid();
		bool ownsRenderPass = false;
	};

	struct ComputePassCacheEntry
	{
		std::string key;
		DeviceRenderStage* stage = nullptr;
	};

	RasterPassCacheEntry* findRasterPassCache(const std::string& key);
	RasterPassCacheEntry& createRasterPassCache(const std::string& key, const RenderGraphRasterPassDesc& desc, RenderGraphResourceHandle frameBufferResource);
	ComputePassCacheEntry* findComputePassCache(const std::string& key);
	ComputePassCacheEntry& createComputePassCache(const std::string& key, const RenderGraphComputePassDesc& desc);
	void addDependency(RenderGraphPassHandle pass, RenderGraphPassHandle dependency);
	void setPassOutput(RenderGraphPassHandle pass, RenderGraphResourceHandle output);
	void setPassOutput(RenderGraphPassHandle pass, const std::string& name, RenderGraphResourceHandle output);
	RenderGraphResourceHandle passOutput(RenderGraphPassHandle pass, const std::string& name) const;
	RenderGraphResourceHandle registerTextureResource(const RenderGraphResourceDesc& desc, DeviceTexture* texture, bool imported);
	void registerFrameBufferAttachments(RenderGraphResourceHandle frameBufferResource);
	RenderGraphResourceHandle findTextureResource(DeviceTexture* texture) const;
	void invalidateRasterPassCache(RenderGraphResourceHandle frameBufferResource);
	bool compilePass(RenderGraphPassHandle pass, std::vector<uint8_t>& visitState, std::string& message);
	void releasePassCache();
	void releaseOwnedResources();
	bool applyAutomaticTransitions(RenderGraphContext& context, const RenderGraphPassDesc& pass, std::string& message);
	bool updateResourceStatesAfterPass(RenderGraphContext& context, const RenderGraphPassDesc& pass, std::string& message);
	bool executeRasterPass(RenderGraphContext& context, RenderGraphPassDesc& pass, std::string& message);
	bool executeIndexedDraws(RenderGraphContext& context, RenderGraphPassDesc& pass, std::string& message);
	void executeComputePass(RenderGraphContext& context, RenderGraphPassDesc& pass);
	bool executeBlitPass(RenderGraphContext& context, RenderGraphPassDesc& pass);
	void executeExternalPass(RenderGraphContext& context, RenderGraphPassDesc& pass);

	std::vector<RenderGraphPassDesc> m_passes;
	std::vector<RenderGraphResource> m_resources;
	uint64_t m_resourceGeneration = 1;
	std::vector<RasterPassCacheEntry> m_rasterPassCache;
	std::vector<ComputePassCacheEntry> m_computePassCache;
	std::vector<std::unique_ptr<DeviceShaderCollection>> m_ownedShaders;
	std::vector<std::string> m_resourceRegistrationErrors;
	std::vector<uint32_t> m_compiledPassOrder;
	bool m_hasCompiledOrder = false;
	RenderGraphPassHandle m_compiledRoot = RenderGraphPassHandle::invalid();
	std::string m_compileMessage;
};
}
