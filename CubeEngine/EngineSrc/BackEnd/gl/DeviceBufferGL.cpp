#include "DeviceBufferGL.h"
#include "../RenderBackEnd.h"
namespace tzw
{

	void DeviceBufferGL::allocate(void* data, size_t ammount)
	{
		bind();
		RenderFlag::BufferTarget glBufferType = RenderFlag::BufferTarget::VertexBuffer;
		switch(m_type)
		{
		case DeviceBufferType::Vertex:
			glBufferType = RenderFlag::BufferTarget::VertexBuffer;
			break;
		case DeviceBufferType::Index:
			glBufferType = RenderFlag::BufferTarget::IndexBuffer;
			break;

		
		}
		RenderBackEnd::shared()->bindBuffer(glBufferType,m_uid);
		RenderBackEnd::shared()->submit(glBufferType,ammount,data, RenderFlag::BufferStorageType::STATIC_DRAW);
	}

	bool DeviceBufferGL::init(DeviceBufferType type)
	{
		m_uid = RenderBackEnd::shared()->genBuffer();
		m_type = type;
		return true;
	}

	void DeviceBufferGL::bind()
	{
		switch(m_type)
		{
		case DeviceBufferType::Vertex:
			RenderBackEnd::shared()->bindBuffer(RenderFlag::BufferTarget::VertexBuffer,m_uid);
			break;
		case DeviceBufferType::Index:
			RenderBackEnd::shared()->bindBuffer(RenderFlag::BufferTarget::IndexBuffer,m_uid);
			break;
		}
	}

	void DeviceBufferGL::setUsePool(bool isUsed)
	{
		//no use, only for vulkan
	}

}