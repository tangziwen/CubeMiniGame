#include "GraphicsRenderer.h"

#include <cstdlib>
#include <cstring>

#include "2D/IMGUISystem.h"
#include "BackEnd/RenderBackEndBase.h"
#include "BackEnd/vk/DeviceFrameBufferVK.h"
#include "BackEnd/vk/DeviceRenderPassVK.h"
#include "Utility/log/Log.h"
#include "CSMShadowSystem.h"
#include "Engine/Engine.h"
#include "RenderPath.h"
#include "RenderQueues.h"
#include "Scene/Scene.h"
#include "Scene/SceneMgr.h"
#include "SceneView.h"
#include "Technique/MaterialInstance.h"

namespace tzw
{
	GraphicsRenderer::GraphicsRenderer()
	{
        m_sceneView = nullptr;
        m_csmShadowSystem = nullptr;
        m_guiQueue = nullptr;
	}

    void GraphicsRenderer::init()
    {
        auto backEnd = Engine::shared()->getRenderBackEnd();

        m_sceneView = new SceneView(m_renderGraph);
        m_sceneView->init();
        m_sceneView->setRenderSettings(&m_renderSettings);
        m_csmShadowSystem = new CSMShadowSystem();
        m_csmShadowSystem->init(m_renderGraph);

        auto thumbnailPass = backEnd->createDeviceRenderpass_imp();
        thumbnailPass->init({{ImageFormat::R8G8B8A8, false}, {ImageFormat::D24_S8, true}}, DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, true);
        m_thumbNailRenderStage = backEnd->createRenderStage_imp();
        m_thumbNailRenderStage->init(thumbnailPass, nullptr);

        for(unsigned int i = 0; i < backEnd->getSwapChainImageCount(); ++i)
        {
            m_screenFrameBuffers.emplace_back(m_renderGraph.importSwapChainFrameBuffer(i));
        }
        m_textureToScreenMat = new MaterialInstance();
        m_textureToScreenMat->loadFromMaterial("TextureToScreen");
        m_renderPath = new RenderPath();
        m_guiQueue = new RenderQueue();
    }

    RenderSettings& GraphicsRenderer::renderSettings()
    {
        return m_renderSettings;
    }

    const RenderSettings& GraphicsRenderer::renderSettings() const
    {
        return m_renderSettings;
    }

    void GraphicsRenderer::initImguiStuff()
    {
        m_imguiMat = new MaterialInstance();
        m_imguiMat->loadFromMaterial("IMGUI");
        ImGuiIO& io = ImGui::GetIO();
        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        RenderGraphResourceDesc desc;
        desc.name = "ImGui.Font";
        desc.format = ImageFormat::R8G8B8A8;
        desc.usage = TextureUsageEnum::SAMPLE_ONLY;
        desc.size = vec2(width, height);
        m_imguiFont = m_renderGraph.createTexture(desc, pixels);
        io.Fonts->TexID = m_renderGraph.texture(m_imguiFont);
        io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    }
    SceneView* GraphicsRenderer::createSceneView(Camera* camera, vec2 size)
    {
        if(!camera || size.x <= 0 || size.y <= 0) return nullptr;
        auto view = new SceneView(m_renderGraph, camera, static_cast<int>(m_additionalSceneViews.size()) + 1, size);
        view->init();
        view->setRenderSettings(&m_renderSettings);
        m_additionalSceneViews.emplace_back(view);
        return view;
    }

    void GraphicsRenderer::preTick()
    {
        if(m_sceneView)
        {
            m_sceneView->preTick(m_renderSettings);
            for(auto view : m_additionalSceneViews) view->preTick(m_renderSettings);
        }
    }
	void GraphicsRenderer::render()
	{
		handleThumbNails();
        auto backEnd = Engine::shared()->getRenderBackEnd();
        backEnd->prepareFrame();
        
		auto cmd = backEnd->getGeneralCommandBuffer();
        cmd->startRecord();
		m_renderPath->prepare(cmd);
        m_sceneView->setRenderSettings(&m_renderSettings);
        for(auto view : m_additionalSceneViews) view->collect();
        m_sceneView->collect();

        m_csmShadowSystem->collect();
        m_renderGraph.beginBuild();
        auto shadows = m_csmShadowSystem->buildRenderGraph();
        std::vector<RenderGraphNode> viewOutputs;
        for(auto view : m_additionalSceneViews)
        {
            view->setShadowNodes(shadows);
            viewOutputs.emplace_back(view->buildRenderGraph());
        }
        m_sceneView->setShadowNodes(shadows);
        auto sceneOutput = m_sceneView->buildRenderGraph();
        for(auto output : viewOutputs) sceneOutput.dependsOn(output);
        auto present = buildScreenGraph(sceneOutput, backEnd->getCurrSwapIndex());
        std::string message;
        RenderGraphContext graphContext(cmd, m_renderPath, nullptr);
        if(m_renderGraph.compile(present, &message) && m_renderGraph.execute(graphContext, &message))
        {
            m_sceneView->onGraphExecuted();
            for(auto view : m_additionalSceneViews) view->onGraphExecuted();
        }
        else
        {
            tlogError("Frame RenderGraph failed:\n%s", message.c_str());
        }
        drawPendingThumbnail(cmd);
		cmd->endRecord();
        backEnd->endFrame(m_renderPath);
	}

    RenderGraphNode GraphicsRenderer::buildScreenGraph(RenderGraphNode sceneOutput, int imageIndex)
    {
        auto target = m_screenFrameBuffers.at(imageIndex);
        auto color = m_renderGraph.colorAttachment(target);
        auto depth = m_renderGraph.depthAttachment(target);
        RenderGraphRasterPassDesc screen;
        screen.name = "Texture To Screen";
        screen.frameBufferResource = target;
        screen.attachments = {{ImageFormat::Surface_Format, false}, {ImageFormat::D24_S8, true}};
        screen.opType = DeviceRenderPass::OpType::LOADCLEAR_AND_STORE;
        screen.material = m_textureToScreenMat;
        RenderGraphPassDesc accesses;
        accesses.readColor(sceneOutput.output()).writeColor(color).writeDepth(depth);
        screen.resourceAccesses = accesses.resourceAccesses;
        auto composite = m_renderGraph.addFullscreenNode(screen, [sceneOutput](RenderGraphPassContext& context)
        {
            context.bindTexture(1, sceneOutput.output());
            context.bindSinglePipelineDescriptor();
            context.drawScreenQuad();
        }).dependsOn(sceneOutput);

        collectUICommands();
        RenderGraphRasterPassDesc gui;
        gui.name = "GUI";
        gui.frameBufferResource = target;
        gui.attachments = screen.attachments;
        gui.opType = DeviceRenderPass::OpType::LOAD_AND_STORE;
        gui.drawPassMask = DrawPassType::GUI;
        gui.consumesSceneQueue = true;
        gui.sceneQueue = m_guiQueue;
        accesses.resourceAccesses.clear();
        accesses.readWriteColor(color).readWriteDepth(depth);
        gui.resourceAccesses = accesses.resourceAccesses;
        auto guiNode = m_renderGraph.addRasterNode(gui, [](RenderGraphPassContext& context)
        {
            context.drawSceneQueue();
        }).dependsOn(composite);
        auto imgui = buildImguiGraph(target, guiNode);
        return m_renderGraph.addPresentNode("Present", color).dependsOn(imgui);
    }

    RenderGraphNode GraphicsRenderer::buildImguiGraph(RenderGraphResourceHandle target, RenderGraphNode previous)
    {
        if(!m_imguiMat) initImguiStuff();
        IMGUISystem::shared()->renderIMGUI();
        const auto drawData = IMGUISystem::shared()->getDrawData();
        if(!drawData || drawData->DisplaySize.x <= 0 || drawData->DisplaySize.y <= 0
            || drawData->FramebufferScale.x <= 0 || drawData->FramebufferScale.y <= 0)
        {
            return previous;
        }
        RenderGraphIndexedDrawData data;
        data.vertexLayout.stride = sizeof(ImDrawVert);
        data.vertexLayout.attributes = {
            {VertexAttributeFormat::Float2, IM_OFFSETOF(ImDrawVert, pos)},
            {VertexAttributeFormat::Float2, IM_OFFSETOF(ImDrawVert, uv)},
            {VertexAttributeFormat::UNorm8x4, IM_OFFSETOF(ImDrawVert, col)}
        };
        static_assert(sizeof(ImDrawIdx) == 2 || sizeof(ImDrawIdx) == 4, "Unsupported ImGui index size");
        data.indexType = sizeof(ImDrawIdx) == 4 ? RenderGraphIndexType::UInt32 : RenderGraphIndexType::UInt16;
        Matrix44 projection;
        projection.ortho(drawData->DisplayPos.x, drawData->DisplayPos.x + drawData->DisplaySize.x,
            drawData->DisplayPos.y + drawData->DisplaySize.y, drawData->DisplayPos.y, 0.1f, 10.0f);
        data.uniformData.resize(sizeof(projection));
        std::memcpy(data.uniformData.data(), &projection, sizeof(projection));
        uint32_t indexOffset = 0;
        int32_t vertexOffset = 0;
        for(int listIndex = 0; listIndex < drawData->CmdListsCount; ++listIndex)
        {
            const auto list = drawData->CmdLists[listIndex];
            if(list->VtxBuffer.Size > 0)
            {
                const auto bytes = reinterpret_cast<const uint8_t*>(list->VtxBuffer.Data);
                data.vertices.insert(data.vertices.end(), bytes, bytes + list->VtxBuffer.Size * sizeof(ImDrawVert));
            }
            if(list->IdxBuffer.Size > 0)
            {
                const auto bytes = reinterpret_cast<const uint8_t*>(list->IdxBuffer.Data);
                data.indices.insert(data.indices.end(), bytes, bytes + list->IdxBuffer.Size * sizeof(ImDrawIdx));
            }
            for(int commandIndex = 0; commandIndex < list->CmdBuffer.Size; ++commandIndex)
            {
                const auto command = &list->CmdBuffer[commandIndex];
                RenderGraphIndexedDraw draw;
                if(command->UserCallback)
                {
                    // Each indexed draw restores the graph pipeline, descriptors, buffers and scissor.
                    if(command->UserCallback != ImDrawCallback_ResetRenderState)
                    {
                        draw.callback = [list, command]() { command->UserCallback(list, command); };
                        data.draws.emplace_back(std::move(draw));
                    }
                    continue;
                }
                if(command->ElemCount == 0) continue;
                draw.texture = command->TextureId
                    ? m_renderGraph.importSampledTexture("ImGui.Texture", static_cast<DeviceTexture*>(command->TextureId))
                    : m_imguiFont;
                draw.indexCount = command->ElemCount;
                draw.firstIndex = indexOffset + command->IdxOffset;
                draw.vertexOffset = vertexOffset + command->VtxOffset;
                const auto origin = drawData->DisplayPos;
                const auto scale = drawData->FramebufferScale;
                draw.scissor = vec4((command->ClipRect.x - origin.x) * scale.x,
                    (command->ClipRect.y - origin.y) * scale.y,
                    (command->ClipRect.z - command->ClipRect.x) * scale.x,
                    (command->ClipRect.w - command->ClipRect.y) * scale.y);
                data.draws.emplace_back(std::move(draw));
            }
            indexOffset += list->IdxBuffer.Size;
            vertexOffset += list->VtxBuffer.Size;
        }
        RenderGraphRasterPassDesc pass;
        pass.name = "ImGui";
        pass.frameBufferResource = target;
        pass.attachments = {{ImageFormat::Surface_Format, false}, {ImageFormat::D24_S8, true}};
        pass.opType = DeviceRenderPass::OpType::LOAD_AND_STORE;
        pass.material = m_imguiMat;
        RenderGraphPassDesc accesses;
        accesses.readWriteColor(m_renderGraph.colorAttachment(target))
            .readWriteDepth(m_renderGraph.depthAttachment(target));
        pass.resourceAccesses = accesses.resourceAccesses;
        return m_renderGraph.addIndexedRasterNode(pass, std::move(data)).dependsOn(previous);
    }

	void GraphicsRenderer::drawPendingThumbnail(DeviceRenderCommand * cmd)
	{
        auto & thumbNailList = getThumbNailList();
	    for(auto thumbnail : thumbNailList)
	    {
		    if(!thumbnail->isIsDone())
		    {
                if(!thumbnail->getFrameBufferVK())
                {
                    thumbnail->initFrameBufferVK(static_cast<DeviceRenderPassVK *>(m_thumbNailRenderStage->getRenderPass()));
                }

                thumbnail->getSnapShotCommand(m_thumbNailRenderStage->getSelfRenderQueue());
                m_thumbNailRenderStage->setFrameBuffer(thumbnail->getFrameBufferVK());
                m_thumbNailRenderStage->prepare(cmd);
                m_thumbNailRenderStage->beginRenderPass(nullptr, vec4(0.5, 0.5, 0.5, 1.0));
                m_thumbNailRenderStage->draw(nullptr, MaterialTechniqueType::Default);
		    	m_thumbNailRenderStage->endRenderPass();
                m_thumbNailRenderStage->finish();
			    thumbnail->setIsDone(true);
                m_renderPath->addRenderStage(m_thumbNailRenderStage);
                break;
		    }
	    }
	}

	void GraphicsRenderer::updateThumbNail(ThumbNail* thumb)
	{
		m_thumbNailList.push_back(thumb);
	}

	std::vector<ThumbNail*>& GraphicsRenderer::getThumbNailList()
	{
		return m_thumbNailList;
	}

	void GraphicsRenderer::handleThumbNails()
	{
		for(auto thumbnail : m_thumbNailList)
		{
			if(!thumbnail->isIsDone())
			{
				thumbnail->doSnapShot();
				//thumbnail->setIsDone(true);
			}
		}
		//m_thumbNailList.clear();
	}

	void GraphicsRenderer::collectUICommands()
	{
		m_guiQueue->clearCommands();
		auto currScene = g_GetCurrScene();
		if(!currScene)
		{
			return;
		}
		std::vector<Node *> directDrawList = currScene->getDirectDrawList();
		for(auto node : directDrawList)
		{
			if(!node || node->getNodeType() != Node::NodeType::DrawableUI)
			{
				continue;
			}
			node->submitDrawCmd(DrawPassType::GUI, m_guiQueue, 0);
			if(node->onSubmitDrawCommand)
			{
				node->onSubmitDrawCommand(DrawPassType::GUI);
			}
		}
	}
}
