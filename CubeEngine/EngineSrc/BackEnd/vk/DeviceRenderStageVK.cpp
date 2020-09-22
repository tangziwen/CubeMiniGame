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
    DeviceBufferVK * DeviceRenderStageVK::m_quadVertexBuffer = nullptr;
    DeviceBufferVK * DeviceRenderStageVK::m_quadIndexBuffer = nullptr;
    Mesh * DeviceRenderStageVK::m_sphere = nullptr;
	DeviceRenderStageVK::DeviceRenderStageVK(DeviceRenderPassVK* renderPass, DeviceFrameBufferVK* frameBuffer)
		:m_renderPass(renderPass),m_frameBuffer(frameBuffer),m_singlePipeline(nullptr)
	{
	}
	DeviceRenderPassVK* DeviceRenderStageVK::getRenderPass()
	{
		return m_renderPass;
	}
	DeviceFrameBufferVK* DeviceRenderStageVK::getFrameBuffer()
	{
		return m_frameBuffer;
	}
	void DeviceRenderStageVK::setRenderPass(DeviceRenderPassVK* renderPass)
	{
		m_renderPass = renderPass;
	}
	void DeviceRenderStageVK::setFrameBuffer(DeviceFrameBufferVK* frameBuffer)
	{
		m_frameBuffer = frameBuffer;
	}

	void DeviceRenderStageVK::prepare()
	{
        m_fuckingObjList.clear();
		fetchCommand();

	}
	void DeviceRenderStageVK::finish()
	{
        //vkCmdEndRenderPass(m_command);
        int res = vkEndCommandBuffer(m_command);
        CHECK_VULKAN_ERROR("vkEndCommandBuffer error %d\n", res);
	}
	void DeviceRenderStageVK::draw(std::vector<RenderCommand>& cmdList)
	{
        for(RenderCommand & a : cmdList)
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

                    currPipeLine = new DevicePipelineVK(this->getFrameBuffer()->getSize(), mat,this->getRenderPass()->getRenderPass(), vertexInput, true, instanceInput, this->getRenderPass()->getAttachmentCount()-1);
                }else //single draw call
                {
                    currPipeLine = new DevicePipelineVK(this->getFrameBuffer()->getSize(), mat, this->getRenderPass()->getRenderPass(), vertexInput, false, instanceInput, this->getRenderPass()->getAttachmentCount()-1);
                }
            
                m_matPipelinePool[mat]  =currPipeLine;
                
            }
            else{
                currPipeLine = iter->second;
            }
            if(m_fuckingObjList.find(currPipeLine) == m_fuckingObjList.end())
            {
                m_fuckingObjList.insert(currPipeLine);
                currPipeLine->collcetItemWiseDescritporSet();
                //update material-wise parameter.
                currPipeLine->updateUniform();
            }
            
            //update uniform.
            DeviceDescriptorVK * itemDescriptorSet = currPipeLine->giveItemWiseDescriptorSet();

            
            DeviceItemBuffer itemBuf = VKRenderBackEnd::shared()->getItemBufferPool()->giveMeItemBuffer(sizeof(ItemUniform));
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
            itemDescriptorSet->updateDescriptorByBinding(0, &itemBuf);
            //recordDrawCommand
            auto vbo = static_cast<DeviceBufferVK *>(mesh->getArrayBuf()->bufferId());
            auto ibo = static_cast<DeviceBufferVK *>(mesh->getIndexBuf()->bufferId());
            if(vbo && ibo)
            {
                vkCmdBindPipeline(m_command, VK_PIPELINE_BIND_POINT_GRAPHICS, currPipeLine->getPipeline());
                VkBuffer vertexBuffers[] = {vbo->getBuffer()};
                VkDeviceSize offsets[] = {0};
                vkCmdBindVertexBuffers(m_command, 0, 1, vertexBuffers, offsets);
            
            
                vkCmdBindIndexBuffer(m_command, ibo->getBuffer(), 0, VK_INDEX_TYPE_UINT16);

                std::vector<VkDescriptorSet> descriptorSetList = {currPipeLine->getMaterialDescriptorSet()->getDescSet(), itemDescriptorSet->getDescSet(),};
                vkCmdBindDescriptorSets(m_command, VK_PIPELINE_BIND_POINT_GRAPHICS, currPipeLine->getPipelineLayOut(), 0, descriptorSetList.size(), descriptorSetList.data(), 0, nullptr);
                if(a.batchType() != RenderCommand::RenderBatchType::Single)
                {
                    auto instancingMesh = a.getInstancedMesh();
                    auto instVBO = static_cast<DeviceBufferVK *>(instancingMesh->getInstanceBuf()->bufferId());
                    VkBuffer instanceVertexBuffers[] = {instVBO->getBuffer()};
                    VkDeviceSize instanceOffsets[] = {0};
                    vkCmdBindVertexBuffers(m_command, 1, 1, instanceVertexBuffers, instanceOffsets);
                    vkCmdDrawIndexed(m_command, static_cast<uint32_t>(mesh->getIndicesSize()), instancingMesh->getInstanceSize(), 0, 0, 0);
                }else
                {
                    vkCmdDrawIndexed(m_command, static_cast<uint32_t>(mesh->getIndicesSize()), 1, 0, 0, 0);
                }
            }
        }
	}

    void DeviceRenderStageVK::drawScreenQuad()
    {
        if(!m_quadIndexBuffer && !m_quadVertexBuffer)
        {
            initFullScreenQuad();
        }
        VkBuffer vertexBuffers[] = {m_quadVertexBuffer->getBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(m_command, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(m_command, m_quadIndexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(m_command, static_cast<uint32_t>(6), 1, 0, 0, 0);
    }

    void DeviceRenderStageVK::drawSphere()
    {
        if(!m_sphere)
        {
            initSphere();
        }
        auto sphereMesh = m_sphere;
        auto vbo = static_cast<DeviceBufferVK *>(sphereMesh->getArrayBuf()->bufferId());
        auto ibo = static_cast<DeviceBufferVK *>(sphereMesh->getIndexBuf()->bufferId());
        VkBuffer vertexBuffers[] = {vbo->getBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(m_command, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(m_command, ibo->getBuffer(), 0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(m_command, static_cast<uint32_t>(sphereMesh->getIndicesSize()), 1, 0, 0, 0);
    }

    VkCommandBuffer DeviceRenderStageVK::getCommand()
    {
        return m_command;
    }

    void DeviceRenderStageVK::createSinglePipeline(Material* material)
    {
        DeviceVertexInput vertexDataInput;
        vertexDataInput.stride = sizeof(VertexData);
        vertexDataInput.addVertexAttributeDesc({VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexData, m_pos)});
        vertexDataInput.addVertexAttributeDesc({VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexData, m_color)});
        vertexDataInput.addVertexAttributeDesc({VK_FORMAT_R32G32_SFLOAT, offsetof(VertexData, m_texCoord)});

        DeviceVertexInput emptyInstancingInput;
        m_singlePipeline = new DevicePipelineVK(getFrameBuffer()->getSize(), material, getRenderPass()->getRenderPass(), vertexDataInput, false, emptyInstancingInput);
    }

    DevicePipelineVK* DeviceRenderStageVK::getSinglePipeline()
    {
        return m_singlePipeline;
    }

    void DeviceRenderStageVK::bindSinglePipelineDescriptor()
    {
        VkDescriptorSet descriptorSetList[] = {m_singlePipeline->getMaterialDescriptorSet()->getDescSet(), };
        vkCmdBindDescriptorSets(m_command, VK_PIPELINE_BIND_POINT_GRAPHICS, m_singlePipeline->getPipelineLayOut(), 0, (sizeof(descriptorSetList) / sizeof(descriptorSetList[0])), descriptorSetList, 0, nullptr);
    }

    void DeviceRenderStageVK::bindSinglePipelineDescriptor(DeviceDescriptorVK* extraItemDescriptor)
    {
        VkDescriptorSet descriptorSetList[] = {m_singlePipeline->getMaterialDescriptorSet()->getDescSet(), extraItemDescriptor->getDescSet() };
        vkCmdBindDescriptorSets(m_command, VK_PIPELINE_BIND_POINT_GRAPHICS, m_singlePipeline->getPipelineLayOut(), 0, (sizeof(descriptorSetList) / sizeof(descriptorSetList[0])), descriptorSetList, 0, nullptr);
    }

    void DeviceRenderStageVK::bindPipeline(DevicePipelineVK* pipeline)
    {
        vkCmdBindPipeline(m_command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline());
    }

    void DeviceRenderStageVK::bindDescriptor(DevicePipelineVK* pipeline, std::vector<DeviceDescriptorVK*> inDescriptorList)
    {
        std::vector<VkDescriptorSet> descriptorSetList;
        for(auto descriptorSet : inDescriptorList)
        {
            descriptorSetList.emplace_back(descriptorSet->getDescSet());
        }
        vkCmdBindDescriptorSets(m_command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipelineLayOut(), 0, descriptorSetList.size(), descriptorSetList.data(), 0, nullptr);
    }

    void DeviceRenderStageVK::beginRenderPass(vec4 clearColor, vec2 clearDepthStencil)
    {
		VkCommandBufferBeginInfo beginInfoDeffered = {};
        beginInfoDeffered.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfoDeffered.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

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
        renderPassInfoDeferred.renderPass = getRenderPass()->getRenderPass();   
        renderPassInfoDeferred.renderArea.offset.x = 0;
        renderPassInfoDeferred.renderArea.offset.y = 0;
        renderPassInfoDeferred.renderArea.extent.width = m_frameBuffer->getSize().x;
        renderPassInfoDeferred.renderArea.extent.height = m_frameBuffer->getSize().y;
        renderPassInfoDeferred.clearValueCount = clearValuesDefferred.size();
        renderPassInfoDeferred.pClearValues = clearValuesDefferred.data();

        int res = vkBeginCommandBuffer(m_command, &beginInfoDeffered);
        CHECK_VULKAN_ERROR("vkBeginCommandBuffer error %d\n", res);
        renderPassInfoDeferred.framebuffer = m_frameBuffer->getFrameBuffer();
        vkCmdBeginRenderPass(m_command, &renderPassInfoDeferred, VK_SUBPASS_CONTENTS_INLINE);

        if(m_singlePipeline)
        {
            vkCmdBindPipeline(getCommand(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_singlePipeline->getPipeline());
            m_singlePipeline->updateUniform();
            m_singlePipeline->collcetItemWiseDescritporSet();
        }

    }

    void DeviceRenderStageVK::endRenderPass()
    {
        vkCmdEndRenderPass(m_command);
    }

    void DeviceRenderStageVK::bindVBO(DeviceBufferVK* buf)
    {
        VkBuffer vertex_buffers[1] = { buf->getBuffer() };
        VkDeviceSize vertex_offset[1] = { 0 };
        vkCmdBindVertexBuffers(m_command, 0, 1, vertex_buffers, vertex_offset);
    }

    void DeviceRenderStageVK::bindIBO(DeviceBufferVK* buf)
    {
        vkCmdBindIndexBuffer(m_command, buf->getBuffer(), 0, VK_INDEX_TYPE_UINT16);
    }

    void DeviceRenderStageVK::setScissor(vec4 scissorRect)
    {
        VkRect2D scissor;
        scissor.offset.x = (int32_t)(scissorRect.x);
        scissor.offset.y = (int32_t)(scissorRect.y);
        scissor.extent.width = (uint32_t)(scissorRect.z);
        scissor.extent.height = (uint32_t)(scissorRect.w);
        vkCmdSetScissor(m_command, 0, 1, &scissor);
    }

    void DeviceRenderStageVK::drawElement(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
    {
        vkCmdDrawIndexed(m_command, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    void DeviceRenderStageVK::initFullScreenQuad()
    {
        VertexData vertices[] = {
            // Vertex data for face 0
            VertexData(vec3(-1.0f, -1.0f,  1.0f), vec2(0.0f, 0.0f)),  // v0
            VertexData(vec3( 1.0f, -1.0f,  1.0f), vec2(1.f, 0.0f)), // v1
            VertexData(vec3(-1.0f,  1.0f,  1.0f), vec2(0.0f, 1.f)),  // v2
            VertexData(vec3( 1.0f,  1.0f,  1.0f), vec2(1.f, 1.f)), // v3
        };
        auto vbuffer = VKRenderBackEnd::shared()->createBuffer_imp();
        vbuffer->init(DeviceBufferType::Vertex);

        vbuffer->allocate(vertices, sizeof(vertices[0]) * 4);
        m_quadVertexBuffer = static_cast<DeviceBufferVK *>(vbuffer);


        uint16_t indices[] = {
         0,  1,  2,  1,  3,  2,

		};
        auto ibuffer = VKRenderBackEnd::shared()->createBuffer_imp();
        ibuffer->init(DeviceBufferType::Index);

        ibuffer->allocate(indices, sizeof(indices));
        m_quadIndexBuffer = static_cast<DeviceBufferVK *>(ibuffer);
    }
	static vec3 pointOnSurface(float u, float v)
	{
        float m_radius = 1.0f;
		return vec3(cos(u) * sin(v) * m_radius, cos(v) * m_radius, sin(u) * sin(v) * m_radius);
	}
    void DeviceRenderStageVK::initSphere()
    {

		m_sphere = new Mesh();
		float PI = 3.1416;
		float startU=0;
		float startV=0;
		float endU=PI*2;
		float endV=PI;
		float stepU=(endU-startU)/24; // step size between U-points on the grid
		float stepV=(endV-startV)/24; // step size between V-points on the grid
		for(int i=0;i<24;i++){ // U-points
			for(int j=0;j<24;j++){ // V-points
				float u=i*stepU+startU;
					float v=j*stepV+startV;
					float un=(i+1==24) ? endU : (i+1)*stepU+startU;
					float vn=(j+1==24) ? endV : (j+1)*stepV+startV;
					// Find the four points of the grid
					// square by evaluating the parametric
					// surface function
					vec3 p0=pointOnSurface(u, v);
					vec3 p1=pointOnSurface(u, vn);
					vec3 p2=pointOnSurface(un, v);
					vec3 p3=pointOnSurface(un, vn);
					// NOTE: For spheres, the normal is just the normalized
					// version of each vertex point; this generally won't be the case for
					// other parametric surfaces.
					// Output the first triangle of this grid square
						
					m_sphere->addVertex(VertexData(p0, p0.normalized(), vec2(u, 1.0 - v)));
					m_sphere->addVertex(VertexData(p2, p2.normalized(), vec2(un, 1.0 - v)));
					m_sphere->addVertex(VertexData(p1, p1.normalized(), vec2(u, 1.0 - vn)));
					m_sphere->addIndex(m_sphere->getIndicesSize());
					m_sphere->addIndex(m_sphere->getIndicesSize());
					m_sphere->addIndex(m_sphere->getIndicesSize());


					m_sphere->addVertex(VertexData(p3, p3.normalized(), vec2(un, 1.0 - vn)));
					m_sphere->addVertex(VertexData(p1, p1.normalized(), vec2(u, 1.0 - vn)));
					m_sphere->addVertex(VertexData(p2, p2.normalized(), vec2(un, 1.0 - v)));
					m_sphere->addIndex(m_sphere->getIndicesSize());
					m_sphere->addIndex(m_sphere->getIndicesSize());
					m_sphere->addIndex(m_sphere->getIndicesSize());
			}
		}
		m_sphere->finish();
    }

    void DeviceRenderStageVK::fetchCommand()
	{
		m_command = VKRenderBackEnd::shared()->getGeneralCommandBuffer();
	}
}