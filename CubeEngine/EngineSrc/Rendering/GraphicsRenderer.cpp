#include "GraphicsRenderer.h"

#include <random>

#include "RenderPath.h"
#include "BackEnd/vk/DeviceRenderStageVK.h"

#include "BackEnd/vk/DevicePipelineVK.h"
#include "BackEnd/vk/DeviceFrameBufferVK.h"
#include "Engine/Engine.h"
#include "BackEnd/RenderBackEndBase.h"

#include "Technique/MaterialPool.h"
#include "3D/ShadowMap/ShadowMap.h"
#include "2D/IMGUISystem.h"
#include "BackEnd/vk/DeviceBufferVK.h"
#include "BackEnd/VkRenderBackEnd.h"
#include <EngineSrc\Scene\SceneMgr.h>
#include "BackEnd/vk/DeviceTextureVK.h"
#include "BackEnd/vk/DeviceShaderCollectionVK.h"
#include "3D/Thumbnail.h"
#include "Scene/SceneMgr.h"
#include "Scene/Scene.h"
#include "Scene/OctreeScene.h"
#include "Engine/DebugSystem.h"
#include "Lighting/PointLight.h"
#include "Scene/SceneCuller.h"
#include "Utility/file/Tfile.h"
#include "RenderQueues.h"
#include "SceneView.h"
#include "ShadowView.h"

namespace tzw
{

    struct PointLightUniform
    {
        alignas(16) Matrix44 wvp;
        alignas(16) vec4 LightPos;
        alignas(16) vec4 LightColor;
    };

	GraphicsRenderer::GraphicsRenderer():m_isAAEnable(true)
	{
        m_sceneView = nullptr;
        m_guiQueue = nullptr;
        for(int i = 0; i < SHADOWMAP_CASCADE_NUM; i++)
        {
            m_shadowViews[i] = nullptr;
        }
	}

    void GraphicsRenderer::init()
    {
        auto backEnd = static_cast<VKRenderBackEnd *>(Engine::shared()->getRenderBackEnd());

        m_sceneView = new SceneView();
        m_sceneView->init();
        for(int i = 0; i < SHADOWMAP_CASCADE_NUM; i++)
        {
            m_shadowViews[i] = new ShadowView(i);
            m_shadowViews[i]->init();
        }

        auto thumbnailPass = backEnd->createDeviceRenderpass_imp();
        thumbnailPass->init({{ImageFormat::R8G8B8A8, false}, {ImageFormat::D24_S8, true}}, DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, true);
        m_thumbNailRenderStage = backEnd->createRenderStage_imp();
        m_thumbNailRenderStage->init(thumbnailPass, nullptr);

        MaterialInstance * matTextureToScreen = new MaterialInstance();
        matTextureToScreen->loadFromMaterial("TextureToScreen");

        for(int i = 0 ; i < 2; i++)
        {
            auto pass = backEnd->createDeviceRenderpass_imp();//new DeviceRenderPassVK(1, DeviceRenderPassVK::OpType::LOADCLEAR_AND_STORE, ImageFormat::Surface_Format, false, true);
            pass->init({{
            ImageFormat::Surface_Format, false}, {
            ImageFormat::D24_S8, true},}, DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, false, true);
            auto frameBuffer = backEnd->createSwapChainFrameBuffer(i);//new DeviceFrameBufferVK(size.x, size.y, m_fbs[i]);
            auto stage = backEnd->createRenderStage_imp();
            stage->init(pass, frameBuffer);
            stage->setName("Texture To Screen Pass");
            m_textureToScreenRenderStage[i] = stage;//new DeviceRenderStageVK(pass, frameBuffer);
            m_textureToScreenRenderStage[i]->createSinglePipeline(matTextureToScreen);
        }

        for(int i = 0 ; i < 2; i++)
        {
            auto pass = backEnd->createDeviceRenderpass_imp();//new DeviceRenderPassVK(1, DeviceRenderPassVK::OpType::LOAD_AND_STORE, ImageFormat::Surface_Format, false, true);
            pass->init({{
            ImageFormat::Surface_Format, false}, {
            ImageFormat::D24_S8, true},}, DeviceRenderPass::OpType::LOAD_AND_STORE, false, true);
            auto frameBuffer = backEnd->createSwapChainFrameBuffer(i);//new DeviceFrameBufferVK(size.x, size.y, m_fbs[i]);
            auto stage = backEnd->createRenderStage_imp();
            stage->init(pass, frameBuffer, (uint32_t)RenderFlag::RenderStage::GUI);
            stage->setName("GUI Pass");
            m_guiStage[i] = stage;//new DeviceRenderStageVK(pass, frameBuffer);
        }
	    m_imguiPipeline = nullptr;
        m_renderPath = new RenderPath();
        m_guiQueue = new RenderQueue();
    }

    void GraphicsRenderer::initImguiStuff()
    {
        auto backEnd = static_cast<VKRenderBackEnd *>(Engine::shared()->getRenderBackEnd());

        m_imguiIndex = backEnd->createBuffer_imp();
        m_imguiIndex->init(DeviceBufferType::Index);
        m_imguiIndex->setAlignment(1024);
        m_imguiIndex->allocateEmpty(1024);

        m_imguiVertex = static_cast<DeviceBufferVK*>(backEnd->createBuffer_imp());
        m_imguiVertex->init(DeviceBufferType::Vertex);
        m_imguiVertex->setAlignment(1024);
        m_imguiVertex->allocateEmpty(1024);

        m_imguiMat = new MaterialInstance();
        m_imguiMat->loadFromMaterial("IMGUI");


        DeviceVertexInput imguiVertexInput;
        imguiVertexInput.stride = sizeof(ImDrawVert);
        imguiVertexInput.addVertexAttributeDesc({VK_FORMAT_R32G32_SFLOAT, IM_OFFSETOF(ImDrawVert, pos)});
        imguiVertexInput.addVertexAttributeDesc({VK_FORMAT_R32G32_SFLOAT, IM_OFFSETOF(ImDrawVert, uv)});
        imguiVertexInput.addVertexAttributeDesc({VK_FORMAT_R8G8B8A8_UNORM, IM_OFFSETOF(ImDrawVert, col)});

        DeviceVertexInput instancingInput;
        vec2 winSize = Engine::shared()->winSize();
        m_imguiPipeline = backEnd->createPipeline_imp();
		m_imguiPipeline->setDynamicState(PIPELINE_DYNAMIC_STATE_FLAG_SCISSOR);
        m_imguiPipeline->init(winSize, m_imguiMat, backEnd->getScreenRenderPass(), imguiVertexInput, false, instancingInput);

        m_imguiMaterial = backEnd->createDeviceMaterial_imp();
        m_imguiMaterial->init(m_imguiMat);

        auto shader = static_cast<DeviceShaderCollectionVK *>(m_imguiMat->getProgram()->getDeviceShader());
        VkDescriptorSetLayout layout = static_cast<DevicePipelineVK*>(m_imguiPipeline)->getDescriptorSetLayOut();
        VkDescriptorSetAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = backEnd->getDescriptorPool();
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &layout;
        auto res = vkAllocateDescriptorSets(backEnd->getDevice(), &alloc_info, &m_imguiDescriptorSet);
        if(res)
        {
            abort();
        }
         printf("create descriptor sets 44444 %p\n", m_imguiDescriptorSet);




        m_imguiUniformBuffer = static_cast<DeviceBufferVK*>(backEnd->createBuffer_imp());
        m_imguiUniformBuffer->init(DeviceBufferType::Uniform);
        m_imguiUniformBuffer->allocateEmpty(sizeof(Matrix44));

        //update descriptor
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_imguiUniformBuffer->getBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(Matrix44);

        VkWriteDescriptorSet writeSet;
        writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeSet.pNext = nullptr;
        writeSet.dstSet = m_imguiDescriptorSet;
        writeSet.dstBinding = 0;
        writeSet.dstArrayElement = 0;
        writeSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeSet.descriptorCount = 1;
        writeSet.pBufferInfo = &bufferInfo;
        VkWriteDescriptorSet writeSetList[] = {writeSet};
        vkUpdateDescriptorSets(backEnd->getDevice(), 1, writeSetList, 0, nullptr);
    

        ImGuiIO& io = ImGui::GetIO();

        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        m_imguiTextureFont = new DeviceTextureVK();
        m_imguiTextureFont->initDataRaw(pixels, width, height, ImageFormat::R8G8B8A8);
        io.Fonts->TexID = m_imguiTextureFont;
        //m_imguiPipeline->getMaterialDescriptorSet()->updateDescriptorByBinding(1, m_imguiTextureFont);
    }
    void GraphicsRenderer::preTick()
    {
        if(m_sceneView)
        {
            m_sceneView->preTick(m_isAAEnable);
        }
    }
	void GraphicsRenderer::render()
	{
		handleThumbNails();
        auto backEnd = static_cast<VKRenderBackEnd *>(Engine::shared()->getRenderBackEnd());
        backEnd->prepareFrame();
        
		auto cmd = backEnd->getGeneralCommandBuffer();
        cmd->startRecord();
		m_renderPath->prepare(cmd);
        m_sceneView->setAAEnabled(m_isAAEnable);
        m_sceneView->collect();

        ShadowMap::shared()->calculateProjectionMatrix();
        std::vector<DeviceTexture *> shadowTextureList;
        for (int i = 0 ; i < SHADOWMAP_CASCADE_NUM ; i++)
        {
            m_shadowViews[i]->collect();
            m_shadowViews[i]->draw(cmd, m_renderPath);
            shadowTextureList.emplace_back(m_shadowViews[i]->depthTexture());
        }

        m_sceneView->setShadowTextures(shadowTextureList);
        m_sceneView->draw(cmd, m_renderPath);

        int imageIdx = backEnd->getCurrSwapIndex();
		DeviceTexture * tex = m_sceneView->outputTexture();

        m_textureToScreenRenderStage[imageIdx]->prepare(cmd);
        m_textureToScreenRenderStage[imageIdx]->beginRenderPass();
        m_textureToScreenRenderStage[imageIdx]->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBinding(1, tex);
        m_textureToScreenRenderStage[imageIdx]->bindSinglePipelineDescriptor();
        m_textureToScreenRenderStage[imageIdx]->drawScreenQuad();
        m_textureToScreenRenderStage[imageIdx]->endRenderPass();
        m_textureToScreenRenderStage[imageIdx]->finish();
        m_renderPath->addRenderStage(m_textureToScreenRenderStage[imageIdx]);
        //------------Texture To Screen Pass end---------------
		

        //------------GUI Pass begin---------------
        collectUICommands();
        m_guiStage[imageIdx]->prepare(cmd);
        m_guiStage[imageIdx]->beginRenderPass();
        m_guiStage[imageIdx]->draw(m_guiQueue, MaterialTechniqueType::Default);
        if(!m_imguiPipeline)
        {
            initImguiStuff();
        }
        //IMGUI
        IMGUISystem::shared()->renderIMGUI();

        auto draw_data = IMGUISystem::shared()->getDrawData();
        int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
        int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
        // Will project scissor/clipping rectangles into framebuffer space
        ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
        ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)
        if (draw_data->TotalVtxCount > 0)
        {
            m_imguiUniformBuffer->map();
		        Matrix44 projection;
                auto screenSize = Engine::shared()->winSize();
		        projection.ortho(0.0f, screenSize.x, screenSize.y, 0.0f, 0.1f, 10.0f);
                m_imguiUniformBuffer->copyFrom(&projection, sizeof(Matrix44));
            m_imguiUniformBuffer->unmap();
            // Create or resize the vertex/index buffers
            size_t vertex_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
            size_t index_size = draw_data->TotalIdxCount * sizeof(ImDrawIdx);
            if (!m_imguiVertex->isValid()|| m_imguiVertex->getSize() < vertex_size)
                m_imguiVertex->allocateEmpty(vertex_size);
                //CreateOrResizeBuffer(rb->VertexBuffer, rb->VertexBufferMemory, rb->VertexBufferSize, vertex_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
            if (!m_imguiIndex->isValid() || m_imguiIndex->getSize() < index_size)
                m_imguiIndex->allocateEmpty(index_size);
                //CreateOrResizeBuffer(rb->IndexBuffer, rb->IndexBufferMemory, rb->IndexBufferSize, index_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

            // Upload vertex/index data into a single contiguous GPU buffer
            size_t vtx_dst_offset = 0;
            size_t idx_dst_offset = 0;
            m_imguiVertex->map();
            m_imguiIndex->map();
            for (int n = 0; n < draw_data->CmdListsCount; n++)
            {
                const ImDrawList* cmd_list = draw_data->CmdLists[n];
                m_imguiVertex->copyFrom(cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), vtx_dst_offset);
                m_imguiIndex->copyFrom(cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), idx_dst_offset);

                vtx_dst_offset += cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
                idx_dst_offset += cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);
            }
            VkMappedMemoryRange range[2] = {};
            range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            range[0].memory = static_cast<DeviceBufferVK *>(m_imguiVertex)->getMemory();
            range[0].size = VK_WHOLE_SIZE;
            range[1].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            range[1].memory = static_cast<DeviceBufferVK *>(m_imguiIndex)->getMemory();
            range[1].size = VK_WHOLE_SIZE;
            VkResult err = vkFlushMappedMemoryRanges(backEnd->getDevice(), 2, range);
            m_imguiVertex->unmap();
            m_imguiIndex->unmap();

            // Render command lists
            // (Because we merged all buffers into a single one, we maintain our own offset into them)
            int global_vtx_offset = 0;
            int global_idx_offset = 0;
            m_imguiPipeline->resetItemWiseDescritporSet();
            for (int n = 0; n < draw_data->CmdListsCount; n++)
            {
                const ImDrawList* cmd_list = draw_data->CmdLists[n];
                for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
                {
                    
                    const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];


                    auto descriptiorSet = m_imguiPipeline->giveItemWiseDescriptorSet();

                    descriptiorSet->updateDescriptorByBinding(0,m_imguiUniformBuffer,0, sizeof(Matrix44));
                    if(pcmd->TextureId)
                    {
                        descriptiorSet->updateDescriptorByBinding(1,static_cast<DeviceTextureVK * >(pcmd->TextureId));
                    }
                    else
                    {
                        descriptiorSet->updateDescriptorByBinding(1,m_imguiTextureFont);
                    }
                    

                    m_guiStage[imageIdx]->bindPipeline(m_imguiPipeline);

                    std::vector<DeviceDescriptor *> descriptorSetList = {m_imguiMaterial->getMaterialDescriptorSet(), descriptiorSet};
                    
                    m_guiStage[imageIdx]->bindDescriptor(m_imguiPipeline, descriptorSetList);

                    m_guiStage[imageIdx]->bindVBO(m_imguiVertex);
                    m_guiStage[imageIdx]->bindIBO(m_imguiIndex);

                    // Project scissor/clipping rectangles into framebuffer space
                    ImVec4 clip_rect;
                    clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
                    clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
                    clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
                    clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

                    if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
                    {
                        // Negative offsets are illegal for vkCmdSetScissor
                        if (clip_rect.x < 0.0f)
                            clip_rect.x = 0.0f;
                        if (clip_rect.y < 0.0f)
                            clip_rect.y = 0.0f;

                        // Apply scissor/clipping rectangle
                        vec4 scissorRect;
                        scissorRect.x = clip_rect.x;
                        scissorRect.y = clip_rect.y;
                        scissorRect.z = clip_rect.z - clip_rect.x;
                        scissorRect.w = clip_rect.w - clip_rect.y;
                        m_guiStage[imageIdx]->setScissor(scissorRect);
                        
                    }
                    // Draw
                    m_guiStage[imageIdx]->drawElement(pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, 0);
                }
                global_idx_offset += cmd_list->IdxBuffer.Size;
                global_vtx_offset += cmd_list->VtxBuffer.Size;
            }
        }

        m_guiStage[imageIdx]->endRenderPass();
        m_guiStage[imageIdx]->finish();
        m_renderPath->addRenderStage(m_guiStage[imageIdx]);
        //------------GUI Pass end---------------

        bool isAnythumbnail = false;
        auto & thumbNailList = getThumbNailList();
	    for(auto thumbnail : thumbNailList)
	    {
		    if(!thumbnail->isIsDone())
		    {
                if(!thumbnail->getFrameBufferVK()){
                    thumbnail->initFrameBufferVK(static_cast<DeviceRenderPassVK *>(m_thumbNailRenderStage->getRenderPass()));
                }
                
                std::vector<RenderCommand> thumbnailCommandList;
                thumbnail->getSnapShotCommand(m_thumbNailRenderStage->getSelfRenderQueue());
                m_thumbNailRenderStage->setFrameBuffer(thumbnail->getFrameBufferVK());
                m_thumbNailRenderStage->prepare(cmd);
                m_thumbNailRenderStage->beginRenderPass(nullptr, vec4(0.5, 0.5, 0.5, 1.0));
                m_thumbNailRenderStage->draw(nullptr, MaterialTechniqueType::Default);
		    	m_thumbNailRenderStage->endRenderPass();
                m_thumbNailRenderStage->finish();
                isAnythumbnail = true;
			    thumbnail->setIsDone(true);
                m_renderPath->addRenderStage(m_thumbNailRenderStage);
                break;
		    }
	    }
		//backEnd->endGeneralCommandBuffer();
		cmd->endRecord();
        backEnd->endFrame(m_renderPath);
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
			node->submitDrawCmd(RenderFlag::RenderStage::GUI, m_guiQueue, 0);
			if(node->onSubmitDrawCommand)
			{
				node->onSubmitDrawCommand(RenderFlag::RenderStage::GUI);
			}
		}
	}
}
