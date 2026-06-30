#pragma once

#include <cstdint>
#include <functional>
#include <limits>
#include <string>
#include <vector>

#include "BackEnd/DeviceRenderPass.h"
#include "BackEnd/DeviceTexture.h"
#include "Math/vec2.h"
#include "Rendering/DrawPass.h"
#include "Technique/MaterialTechnique.h"

namespace tzw
{
class DeviceDescriptor;
class DeviceFrameBuffer;
class DeviceMaterial;
class DevicePipeline;
class DeviceRenderCommand;
class DeviceRenderStage;
class MaterialInstance;
class RenderGraph;
class RenderGraphContext;
class RenderPath;
class RenderQueue;
struct RenderGraphPassDesc;
struct RenderGraphResource;

enum class RenderGraphResourceLayout
{
	Unknown,
	ColorAttachment,
	DepthAttachment,
	DepthRead,
	ShaderRead,
	TransferSrc,
	TransferDst,
	General,
};

enum class RenderGraphResourceAccessType
{
	ReadColor,
	ReadDepth,
	WriteColor,
	WriteDepth,
	ReadWriteColor,
	TransferRead,
	TransferWrite,
};

class RenderGraphResourceHandle
{
public:
	RenderGraphResourceHandle();

	bool isValid() const;
	uint32_t index() const;
	static RenderGraphResourceHandle invalid();

private:
	friend class RenderGraph;

	explicit RenderGraphResourceHandle(uint32_t index);

	uint32_t m_index;
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

struct RenderGraphResourceDesc
{
	std::string name;
	ImageFormat format = ImageFormat::Surface_Format;
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
	Blit,
	External,
};

struct RenderGraphResource
{
	RenderGraphResourceDesc desc;
	DeviceTexture* texture = nullptr;
	DeviceTexture* depthTexture = nullptr;
	DeviceFrameBuffer* frameBuffer = nullptr;
	DeviceRenderPass* renderPass = nullptr;
	bool graphOwned = false;
	RenderGraphResourceLayout currentColorLayout = RenderGraphResourceLayout::Unknown;
	RenderGraphResourceLayout currentDepthLayout = RenderGraphResourceLayout::Unknown;
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
	std::vector<RenderGraphResourceAccess> resourceAccesses;
};

class RenderGraphPassContext
{
public:
	DeviceRenderCommand* cmd() const;
	RenderPath* renderPath() const;
	RenderQueue* sceneQueue() const;
	const RenderGraphPassDesc* pass() const;
	const RenderGraphResource* resource(RenderGraphResourceHandle handle) const;
	DeviceTexture* texture(RenderGraphResourceHandle handle) const;
	DeviceTexture* depthTexture(RenderGraphResourceHandle handle) const;
	DeviceFrameBuffer* frameBuffer(RenderGraphResourceHandle handle) const;
	DeviceFrameBuffer* targetFrameBuffer() const;
	DeviceMaterial* material() const;
	DeviceDescriptor* materialDescriptor() const;
	DevicePipeline* pipeline() const;
	DeviceDescriptor* itemDescriptor() const;
	void bindSinglePipelineDescriptor();
	void bindSinglePipelineDescriptor(DeviceDescriptor* extraItemDescriptor);
	void drawQueue(RenderQueue* renderQueue, MaterialTechniqueType techniqueType = MaterialTechniqueType::Default);
	void drawSceneQueue(MaterialTechniqueType techniqueType = MaterialTechniqueType::Default);
	void drawScreenQuad();
	void drawSphere();

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
	DeviceTexture* texture(RenderGraphResourceHandle handle) const;
	DeviceTexture* depthTexture(RenderGraphResourceHandle handle) const;
	DeviceFrameBuffer* frameBuffer(RenderGraphResourceHandle handle) const;

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
	RenderGraphPassDesc& readWriteColor(RenderGraphResourceHandle resource, RenderGraphResourceLayout finalLayout = RenderGraphResourceLayout::Unknown);
	RenderGraphPassDesc& transferRead(RenderGraphResourceHandle resource);
	RenderGraphPassDesc& transferWrite(RenderGraphResourceHandle resource);

	std::string name;
	DrawPassTypeMask consumedDrawPassMask = DrawPassType::Unset;
	std::vector<RenderGraphResourceAccess> resourceAccesses;
	std::function<void(RenderGraphPassContext&)> execute;

private:
	friend class RenderGraph;
	friend class RenderGraphPassContext;

	RenderGraphPassKind kind = RenderGraphPassKind::External;
	RenderGraphPassHandle handle = RenderGraphPassHandle::invalid();
	DeviceRenderPass* renderPass = nullptr;
	DeviceRenderStage* compiledStage = nullptr;
	RenderGraphResourceHandle frameBufferResource = RenderGraphResourceHandle::invalid();
	RenderGraphResourceHandle blitSource = RenderGraphResourceHandle::invalid();
	RenderGraphResourceHandle blitDestination = RenderGraphResourceHandle::invalid();
};

class RenderGraph
{
public:
	void beginBuild();
	void clear();
	void clearResources();
	RenderGraphPassHandle addPass(const RenderGraphPassDesc& desc);
	RenderGraphPassHandle addRasterPass(const RenderGraphRasterPassDesc& desc, std::function<void(RenderGraphPassContext&)> execute = nullptr);
	RenderGraphPassHandle addFullscreenPass(const RenderGraphRasterPassDesc& desc, std::function<void(RenderGraphPassContext&)> execute = nullptr);
	RenderGraphPassHandle addBlitPass(const std::string& name, RenderGraphResourceHandle source, RenderGraphResourceHandle destination);
	RenderGraphPassHandle addExternalPass(const RenderGraphPassDesc& desc);
	RenderGraphResourceHandle importTexture(const RenderGraphResourceDesc& desc, DeviceTexture* texture);
	RenderGraphResourceHandle importFrameBuffer(const RenderGraphResourceDesc& desc, DeviceFrameBuffer* frameBuffer);
	RenderGraphResourceHandle createFrameBuffer(const RenderGraphResourceDesc& desc, DeviceRenderPass* renderPass);
	RenderGraphResourceHandle createFrameBuffer(const RenderGraphResourceDesc& desc, const DeviceAttachmentInfoList& attachments,
		DeviceRenderPass::OpType opType, bool isNeedTransitionToRead, bool isOutputToScreen = false);
	void resizeOwnedFrameBuffers(vec2 size);
	void setResourceLayout(RenderGraphResourceHandle handle, RenderGraphResourceLayout colorLayout, RenderGraphResourceLayout depthLayout = RenderGraphResourceLayout::Unknown);
	DeviceFrameBuffer* passFrameBuffer(RenderGraphPassHandle handle) const;
	RenderGraphResource* resource(RenderGraphResourceHandle handle);
	const RenderGraphResource* resource(RenderGraphResourceHandle handle) const;
	DeviceTexture* texture(RenderGraphResourceHandle handle) const;
	DeviceTexture* depthTexture(RenderGraphResourceHandle handle) const;
	DeviceFrameBuffer* frameBuffer(RenderGraphResourceHandle handle) const;
	bool validate(std::string* outMessage = nullptr) const;
	std::string dump() const;
	void execute(RenderGraphContext& context);

private:
	struct RasterPassCacheEntry
	{
		std::string key;
		DeviceRenderPass* renderPass = nullptr;
		DeviceRenderStage* stage = nullptr;
		RenderGraphResourceHandle frameBufferResource = RenderGraphResourceHandle::invalid();
	};

	RasterPassCacheEntry* findRasterPassCache(const std::string& key);
	RasterPassCacheEntry& createRasterPassCache(const std::string& key, const RenderGraphRasterPassDesc& desc, RenderGraphResourceHandle frameBufferResource);
	void releaseOwnedResources();
	void applyAutomaticTransitions(RenderGraphContext& context, const RenderGraphPassDesc& pass);
	void updateResourceLayoutsAfterPass(const RenderGraphPassDesc& pass);
	void executeRasterPass(RenderGraphContext& context, RenderGraphPassDesc& pass);
	void executeBlitPass(RenderGraphContext& context, RenderGraphPassDesc& pass);
	void executeExternalPass(RenderGraphContext& context, RenderGraphPassDesc& pass);

	std::vector<RenderGraphPassDesc> m_passes;
	std::vector<RenderGraphResource> m_resources;
	std::vector<RasterPassCacheEntry> m_rasterPassCache;
};
}
