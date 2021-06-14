#include "DeviceRenderPassVK.h"
#include "../VkRenderBackEnd.h"
#include "DeviceTextureVK.h"
#include "DeviceFrameBufferVK.h"
#include "DeviceRenderStageVK.h"
#include "EngineSrc/Mesh/VertexData.h"
#include "Rendering/RenderCommand.h"
#include "BackEnd/vk/DeviceBufferVK.h"
#include "Mesh/InstancedMesh.h"
namespace tzw
{
	DeviceRenderStageVK::DeviceRenderStageVK()
	{

	}

	void DeviceRenderStageVK::finish()
	{
        //int res = vkEndCommandBuffer(m_command);
        //CHECK_VULKAN_ERROR("vkEndCommandBuffer error %d\n", res);
	}

	void DeviceRenderStageVK::draw(RenderQueue * renderQueue)
	{
		if(renderQueue)
		{
			renderQueue->dispatch(m_selfRenderQueue, m_renderStage);
		}
        for(RenderCommand & a : m_selfRenderQueue->getList())
        {

            //if(a.batchType() != RenderCommand::RenderBatchType::Single) continue;
            Material * mat = a.getMat();

            //std::string & matStr = mat->getFullDescriptionStr();
            DevicePipelineVK * currPipeLine;
            auto iter = m_matPipelinePool.find(mat);
            if(iter == m_matPipelinePool.end())
            {
                DeviceVertexInput vertexInput;
                vertexInput.stride = sizeof(VertexData);
                vertexInput.addVertexAttributeDesc({VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexData, m_pos)});
                vertexInput.addVertexAttributeDesc({VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexData, m_color)});
                vertexInput.addVertexAttributeDesc({VK_FORMAT_R32G32_SFLOAT, offsetof(VertexData, m_texCoord)});
                vertexInput.addVertexAttributeDesc({VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexData, m_normal)});
                vertexInput.addVertexAttributeDesc({VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexData, m_tangent)});

                //use for terrain
                vertexInput.addVertexAttributeDesc({VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexData, m_matBlendFactor)});
                vertexInput.addVertexAttributeDesc({VK_FORMAT_R8G8B8_UINT, offsetof(VertexData, m_matIndex)});
                //instancing optional
                DeviceVertexInput instanceInput;
                //instancing
                if(a.batchType() != RenderCommand::RenderBatchType::Single)
                {
                    instanceInput.stride = sizeof(InstanceData);
                    //matrix
                    instanceInput.addVertexAttributeDesc({VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceData, transform)});
                    instanceInput.addVertexAttributeDesc({VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceData, transform) + sizeof(float) * 4});
                    instanceInput.addVertexAttributeDesc({VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceData, transform)+ sizeof(float) * 8});
                    instanceInput.addVertexAttributeDesc({VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceData, transform)+ sizeof(float) * 12});

                    instanceInput.addVertexAttributeDesc({VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceData, extraInfo.x)});

                    currPipeLine = new DevicePipelineVK();
                    currPipeLine->init(this->getFrameBuffer()->getSize(), mat,this->getRenderPass(), vertexInput, true, instanceInput, this->getRenderPass()->getAttachmentCount()-1);
                }else //single draw call
                {
                    currPipeLine = new DevicePipelineVK();
                    currPipeLine->init(this->getFrameBuffer()->getSize(), mat, this->getRenderPass(), vertexInput, false, instanceInput, this->getRenderPass()->getAttachmentCount()-1);
                }
            
                m_matPipelinePool[mat]  =currPipeLine;
                
            }
            else{
                currPipeLine = static_cast<DevicePipelineVK*>(iter->second);
            }
            if(m_fuckingObjList.find(currPipeLine) == m_fuckingObjList.end())
            {
                m_fuckingObjList.insert(currPipeLine);
                //update material-wise parameter.
                currPipeLine->updateUniform();
                currPipeLine->updateMaterialDescriptorSet();
            }
            

            //update uniform.
            DeviceRenderItem * renderItem = currPipeLine->getRenderItem(a.getDrawableObj());
            DeviceDescriptor * itemDescriptorSet = renderItem->m_desc;//currPipeLine->giveItemWiseDescriptorSet();
            //DeviceDescriptor * itemDescriptorSet = currPipeLine->giveItemWiseDescriptorSet();

            
            DeviceItemBuffer itemBuf = renderItem->m_buff;//VKRenderBackEnd::shared()->getItemBufferPool()->giveMeItemBuffer(sizeof(ItemUniform));
            //DeviceItemBuffer itemBuf = VKRenderBackEnd::shared()->getItemBufferPool()->giveMeItemBuffer(sizeof(ItemUniform));
            //itemDescriptorSet->updateDescriptorByBinding(0, &itemBuf);
            itemBuf.map();
                ItemUniform uniformStruct;
                uniformStruct.wvp = a.m_transInfo.m_projectMatrix * (a.m_transInfo.m_viewMatrix  * a.m_transInfo.m_worldMatrix );
                uniformStruct.wv = a.m_transInfo.m_viewMatrix  * a.m_transInfo.m_worldMatrix;
                uniformStruct.world = a.m_transInfo.m_worldMatrix;
                uniformStruct.view = a.m_transInfo.m_viewMatrix;
                uniformStruct.projection = a.m_transInfo.m_projectMatrix;
            itemBuf.copyFrom(&uniformStruct, sizeof(uniformStruct));
            itemBuf.unMap();
		    Mesh * mesh = nullptr;
            if(a.batchType() != RenderCommand::RenderBatchType::Single)
            {
                mesh = a.getInstancedMesh()->getMesh();
            }else
            {
                mesh = a.getMesh();
            }
            
            //recordDrawCommand
            auto vbo = static_cast<DeviceBufferVK *>(mesh->getArrayBuf()->bufferId());
            auto ibo = static_cast<DeviceBufferVK *>(mesh->getIndexBuf()->bufferId());

        	DeviceRenderCommandVK * command = static_cast<DeviceRenderCommandVK*>(m_deviceRenderCommand);
            if(vbo && ibo)
            {
                vkCmdBindPipeline(command->getVK(), VK_PIPELINE_BIND_POINT_GRAPHICS, currPipeLine->getPipeline());
                VkBuffer vertexBuffers[] = {vbo->getBuffer()};
                VkDeviceSize offsets[] = {0};
                vkCmdBindVertexBuffers(command->getVK(), 0, 1, vertexBuffers, offsets);
            
            
                vkCmdBindIndexBuffer(command->getVK(), ibo->getBuffer(), 0, VK_INDEX_TYPE_UINT16);

                std::vector<VkDescriptorSet> descriptorSetList = {static_cast<DeviceDescriptorVK*>(currPipeLine->getMaterialDescriptorSet())->getDescSet(), 
                    static_cast<DeviceDescriptorVK*>(itemDescriptorSet)->getDescSet(),};
                vkCmdBindDescriptorSets(command->getVK(), VK_PIPELINE_BIND_POINT_GRAPHICS, currPipeLine->getPipelineLayOut(), 0, descriptorSetList.size(), descriptorSetList.data(), 0, nullptr);
                if(a.batchType() != RenderCommand::RenderBatchType::Single)
                {
                    auto instancingMesh = a.getInstancedMesh();
                    auto instVBO = static_cast<DeviceBufferVK *>(instancingMesh->getInstanceBuf()->bufferId());
                    VkBuffer instanceVertexBuffers[] = {instVBO->getBuffer()};
                    VkDeviceSize instanceOffsets[] = {0};
                    vkCmdBindVertexBuffers(command->getVK(), 1, 1, instanceVertexBuffers, instanceOffsets);
                    vkCmdDrawIndexed(command->getVK(), static_cast<uint32_t>(mesh->getIndicesSize()), instancingMesh->getInstanceSize(), 0, 0, 0);
                }else
                {
                    vkCmdDrawIndexed(command->getVK(), static_cast<uint32_t>(mesh->getIndicesSize()), 1, 0, 0, 0);
                }
            }
        }
	}

    void DeviceRenderStageVK::drawScreenQuad()
    {
		DeviceRenderCommandVK * command = static_cast<DeviceRenderCommandVK*>(m_deviceRenderCommand);
        if(!m_quadIndexBuffer && !m_quadVertexBuffer)
        {
            initFullScreenQuad();
        }
        VkBuffer vertexBuffers[] = {static_cast<DeviceBufferVK*>(m_quadVertexBuffer)->getBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(command->getVK(), 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(command->getVK(), static_cast<DeviceBufferVK*>(m_quadIndexBuffer)->getBuffer(), 0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(command->getVK(), static_cast<uint32_t>(6), 1, 0, 0, 0);
    }

    void DeviceRenderStageVK::drawSphere()
    {
        if(!m_sphere)
        {
            initSphere();
        }
		DeviceRenderCommandVK * command = static_cast<DeviceRenderCommandVK*>(m_deviceRenderCommand);
        auto sphereMesh = m_sphere;
        auto vbo = static_cast<DeviceBufferVK *>(sphereMesh->getArrayBuf()->bufferId());
        auto ibo = static_cast<DeviceBufferVK *>(sphereMesh->getIndexBuf()->bufferId());
        VkBuffer vertexBuffers[] = {vbo->getBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(command->getVK(), 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(command->getVK(), ibo->getBuffer(), 0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(command->getVK(), static_cast<uint32_t>(sphereMesh->getIndicesSize()), 1, 0, 0, 0);
    }

    void DeviceRenderStageVK::bindSinglePipelineDescriptor()
    {
		DeviceRenderCommandVK * command = static_cast<DeviceRenderCommandVK*>(m_deviceRenderCommand);
        VkDescriptorSet descriptorSetList[] = {static_cast<DeviceDescriptorVK*>(m_singlePipeline->getMaterialDescriptorSet())->getDescSet(), };
        vkCmdBindDescriptorSets(command->getVK(), VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<DevicePipelineVK*>(m_singlePipeline)->getPipelineLayOut(), 0, (sizeof(descriptorSetList) / sizeof(descriptorSetList[0])), descriptorSetList, 0, nullptr);
    }

    void DeviceRenderStageVK::bindSinglePipelineDescriptor(DeviceDescriptor* extraItemDescriptor)
    {
		DeviceRenderCommandVK * command = static_cast<DeviceRenderCommandVK*>(m_deviceRenderCommand);
        VkDescriptorSet descriptorSetList[] = {static_cast<DeviceDescriptorVK*>(m_singlePipeline->getMaterialDescriptorSet())->getDescSet(), static_cast<DeviceDescriptorVK *>(extraItemDescriptor)->getDescSet() };
        vkCmdBindDescriptorSets(command->getVK(), VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<DevicePipelineVK*>(m_singlePipeline)->getPipelineLayOut(), 0, (sizeof(descriptorSetList) / sizeof(descriptorSetList[0])), descriptorSetList, 0, nullptr);
    }

    void DeviceRenderStageVK::bindPipeline(DevicePipeline* pipeline)
    {
		DeviceRenderCommandVK * command = static_cast<DeviceRenderCommandVK*>(m_deviceRenderCommand);
        vkCmdBindPipeline(command->getVK(), VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<DevicePipelineVK *>(pipeline)->getPipeline());
    }

    void DeviceRenderStageVK::bindDescriptor(DevicePipeline* pipeline, std::vector<DeviceDescriptor*> inDescriptorList)
    {
        std::vector<VkDescriptorSet> descriptorSetList;
        for(auto descriptorSet : inDescriptorList)
        {
            descriptorSetList.emplace_back(static_cast<DeviceDescriptorVK *>(descriptorSet)->getDescSet());
        }
		DeviceRenderCommandVK * command = static_cast<DeviceRenderCommandVK*>(m_deviceRenderCommand);
        vkCmdBindDescriptorSets(command->getVK(), VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<DevicePipelineVK *>(pipeline)->getPipelineLayOut(), 0, descriptorSetList.size(), descriptorSetList.data(), 0, nullptr);
    }

    void DeviceRenderStageVK::beginRenderPass(vec4 clearColor, vec2 clearDepthStencil)
    {
		auto attachmentList = m_renderPass->getAttachmentList();
		std::vector<VkClearValue> clearValuesDefferred{};
		clearValuesDefferred.resize(attachmentList.size());
		for(int i = 0; i < attachmentList.size(); i++)
		{
			if(attachmentList[i].isDepth){
			
				clearValuesDefferred[i].depthStencil = {clearDepthStencil.x, (uint32_t)clearDepthStencil.y};
			}
			else
			{
				clearValuesDefferred[i].color = {clearColor.x, clearColor.y, clearColor.z, clearColor.w};
			}
		}

		VkRenderPassBeginInfo renderPassInfoDeferred = {};
        renderPassInfoDeferred.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfoDeferred.renderPass = static_cast<DeviceRenderPassVK*>(getRenderPass())->getRenderPass();   
        renderPassInfoDeferred.renderArea.offset.x = 0;
        renderPassInfoDeferred.renderArea.offset.y = 0;
        renderPassInfoDeferred.renderArea.extent.width = m_frameBuffer->getSize().x;
        renderPassInfoDeferred.renderArea.extent.height = m_frameBuffer->getSize().y;
        renderPassInfoDeferred.clearValueCount = clearValuesDefferred.size();
        renderPassInfoDeferred.pClearValues = clearValuesDefferred.data();

        DeviceRenderCommandVK * command = static_cast<DeviceRenderCommandVK*>(m_deviceRenderCommand);
        VKRenderBackEnd::shared()->beginDebugRegion(command->getVK(), m_name.c_str());
        
        renderPassInfoDeferred.framebuffer = static_cast<DeviceFrameBufferVK*>(m_frameBuffer)->getFrameBuffer();
        vkCmdBeginRenderPass(command->getVK(), &renderPassInfoDeferred, VK_SUBPASS_CONTENTS_INLINE);

        if(m_singlePipeline)
        {
            vkCmdBindPipeline(command->getVK(), VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<DevicePipelineVK*>(m_singlePipeline)->getPipeline());
            static_cast<DevicePipelineVK*>(m_singlePipeline)->updateUniform();
            static_cast<DevicePipelineVK*>(m_singlePipeline)->resetItemWiseDescritporSet();
        }

    }

    void DeviceRenderStageVK::endRenderPass()
    {
		DeviceRenderCommandVK * command = static_cast<DeviceRenderCommandVK*>(m_deviceRenderCommand);
        vkCmdEndRenderPass(command->getVK());
        VKRenderBackEnd::shared()->endDebugRegion(command->getVK());
    }

    void DeviceRenderStageVK::bindVBO(DeviceBuffer* buf)
    {
        VkBuffer vertex_buffers[1] = { static_cast<DeviceBufferVK *>(buf)->getBuffer() };
        VkDeviceSize vertex_offset[1] = { 0 };
		DeviceRenderCommandVK * command = static_cast<DeviceRenderCommandVK*>(m_deviceRenderCommand);
        vkCmdBindVertexBuffers(command->getVK(), 0, 1, vertex_buffers, vertex_offset);
    }

    void DeviceRenderStageVK::bindIBO(DeviceBuffer* buf)
    {
		DeviceRenderCommandVK * command = static_cast<DeviceRenderCommandVK*>(m_deviceRenderCommand);
        vkCmdBindIndexBuffer(command->getVK(), static_cast<DeviceBufferVK *>(buf)->getBuffer(), 0, VK_INDEX_TYPE_UINT16);
    }

    void DeviceRenderStageVK::setScissor(vec4 scissorRect)
    {
        VkRect2D scissor;
        scissor.offset.x = (int32_t)(scissorRect.x);
        scissor.offset.y = (int32_t)(scissorRect.y);
        scissor.extent.width = (uint32_t)(scissorRect.z);
        scissor.extent.height = (uint32_t)(scissorRect.w);
		DeviceRenderCommandVK * command = static_cast<DeviceRenderCommandVK*>(m_deviceRenderCommand);
        vkCmdSetScissor(command->getVK(), 0, 1, &scissor);
    }

    void DeviceRenderStageVK::drawElement(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
    {
		DeviceRenderCommandVK * command = static_cast<DeviceRenderCommandVK*>(m_deviceRenderCommand);
        vkCmdDrawIndexed(command->getVK(), indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    void DeviceRenderStageVK::fetchCommand()
	{
		//m_command = VKRenderBackEnd::shared()->getGeneralCommandBuffer();
	}
}