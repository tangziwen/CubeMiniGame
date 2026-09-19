#include "BackEnd/DeviceFrameBuffer.h"
#include "BackEnd/DeviceRenderCommand.h"
#include "BackEnd/DeviceTexture.h"
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
	desc.format = ImageFormat::R8G8B8A8;
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
	conflictDesc.format = ImageFormat::D16;
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
	expect(graph.texture(colorHandle) == &newTexture, "rebind must update the attachment texture pointer");
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
	expect(graph.texture(graph.colorAttachment(secondHandle)) == &shared && graph.frameBuffer(firstHandle) == &first,
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
	expect(graph.frameBuffer(multipleHandle) == &multiple, "duplicate attachment rejection must preserve the framebuffer");
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
	renderPass.init({{ImageFormat::R8G8B8A8, false}, {ImageFormat::D24_S8, true}}, DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, true);
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

}
}

int main()
{
	using namespace tzw;
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
	if(failureCount != 0)
	{
		std::cerr << failureCount << " RenderGraph test(s) failed.\n";
		return 1;
	}
	std::cout << "All RenderGraph tests passed.\n";
	return 0;
}
