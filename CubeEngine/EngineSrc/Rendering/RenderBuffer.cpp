#include "RenderBuffer.h"
//#include "../BackEnd/RenderBackEnd.h"
#include "BackEnd/RenderBackEndBase.h"
#include "Engine/Engine.h"
namespace tzw {

RenderBuffer::RenderBuffer(Type bufferType)
    :m_type(bufferType),m_bufferId(0),m_amount(0)
{

}

void RenderBuffer::create()
{
    m_bufferId = Engine::shared()->getRenderBackEnd()->createBuffer_imp();
    DeviceBufferType targetType = DeviceBufferType::Vertex;
    switch(m_type)
    {
    case Type::INDEX:
        targetType = DeviceBufferType::Index;
        break;
    case Type::VERTEX:
        targetType = DeviceBufferType::Vertex;
        break;
    }
    m_bufferId->init(targetType);

    m_bufferId->setUsePool(true);
}

void RenderBuffer::allocate(void *data, unsigned int amount, RenderFlag::BufferStorageType storageType)
{
    m_bufferId->allocate(data, amount);
	m_amount = amount;
}


void RenderBuffer::copyData(void* data, size_t dataSize)
{
    m_bufferId->map();
    m_bufferId->copyFrom(data, dataSize, 0);
    m_bufferId->unmap();
}

void RenderBuffer::use()
{
    m_bufferId->bind();
}
DeviceBuffer * RenderBuffer::bufferId() const
{
    return m_bufferId;
}

unsigned RenderBuffer::getAmount() const
{
	return m_amount;
}

RenderBuffer::~RenderBuffer()
{
	delete m_bufferId;
}
} // namespace tzw

