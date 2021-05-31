#include "DeviceRenderPass.h"
#include "VkRenderBackEnd.h"
#include "DeviceTexture.h"
#include "DeviceFrameBuffer.h"
#include "DeviceRenderStage.h"
#include "EngineSrc/Mesh/VertexData.h"
#include "Rendering/RenderCommand.h"
#include "DeviceBuffer.h"
#include "Mesh/InstancedMesh.h"
namespace tzw
{
    DeviceBuffer * DeviceRenderStage::m_quadVertexBuffer = nullptr;
    DeviceBuffer * DeviceRenderStage::m_quadIndexBuffer = nullptr;
    Mesh * DeviceRenderStage::m_sphere = nullptr;
	void DeviceRenderStage::init(DeviceRenderPass* renderPass, DeviceFrameBuffer* frameBuffer)
	{
        m_renderPass = renderPass;
        m_frameBuffer = frameBuffer;
        m_singlePipeline = nullptr;
	}
	DeviceRenderPass* DeviceRenderStage::getRenderPass()
	{
		return m_renderPass;
	}
	DeviceFrameBuffer* DeviceRenderStage::getFrameBuffer()
	{
		return m_frameBuffer;
	}
	void DeviceRenderStage::setRenderPass(DeviceRenderPass* renderPass)
	{
		m_renderPass = renderPass;
	}
	void DeviceRenderStage::setFrameBuffer(DeviceFrameBuffer* frameBuffer)
	{
		m_frameBuffer = frameBuffer;
	}

	void DeviceRenderStage::prepare(DeviceRenderCommand* renderCommand)
	{
        m_fuckingObjList.clear();
		//fetchCommand();
		m_deviceRenderCommand = renderCommand;

	}

    void DeviceRenderStage::createSinglePipeline(Material* material)
    {
        DeviceVertexInput vertexDataInput;
        vertexDataInput.stride = sizeof(VertexData);
        vertexDataInput.addVertexAttributeDesc({VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexData, m_pos)});
        vertexDataInput.addVertexAttributeDesc({VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexData, m_color)});
        vertexDataInput.addVertexAttributeDesc({VK_FORMAT_R32G32_SFLOAT, offsetof(VertexData, m_texCoord)});

        DeviceVertexInput emptyInstancingInput;
		m_singlePipeline = VKRenderBackEnd::shared()->createPipeline_imp();
        m_singlePipeline->init(getFrameBuffer()->getSize(), material, getRenderPass(), vertexDataInput, false, emptyInstancingInput);
    }

    DevicePipeline* DeviceRenderStage::getSinglePipeline()
    {
        return m_singlePipeline;
    }

    void DeviceRenderStage::initFullScreenQuad()
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
        m_quadVertexBuffer = vbuffer;//static_cast<DeviceBufferVK *>(vbuffer);


        uint16_t indices[] = {
         0,  1,  2,  1,  3,  2,

		};
        auto ibuffer = VKRenderBackEnd::shared()->createBuffer_imp();
        ibuffer->init(DeviceBufferType::Index);

        ibuffer->allocate(indices, sizeof(indices));
        m_quadIndexBuffer = ibuffer;//static_cast<DeviceBufferVK *>(ibuffer);
    }
	static vec3 pointOnSurface(float u, float v)
	{
        float m_radius = 1.0f;
		return vec3(cos(u) * sin(v) * m_radius, cos(v) * m_radius, sin(u) * sin(v) * m_radius);
	}
    void DeviceRenderStage::initSphere()
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
}