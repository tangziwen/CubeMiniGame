#include "RenderBuffer.h"
#include "../BackEnd/RenderBackEnd.h"
namespace tzw {

RenderBuffer::RenderBuffer(Type bufferType)
    :m_type(bufferType),m_bufferId(0),m_amount(0)
{

}

void RenderBuffer::create()
{
    m_bufferId = RenderBackEnd::shared()->genBuffer();
}

void RenderBuffer::allocate(void *data, unsigned int amount, RenderFlag::BufferStorageType storageType)
{
    use();
	m_amount = amount;
    switch(m_type)
    {
    case Type::INDEX:
        RenderBackEnd::shared()->submit(RenderFlag::BufferTarget::IndexBuffer,amount,data, storageType);
        break;
    case Type::VERTEX:
        RenderBackEnd::shared()->submit(RenderFlag::BufferTarget::VertexBuffer,amount,data, storageType);
        break;
    }
}

void RenderBuffer::resubmit(void* data, unsigned offset, unsigned amount)
{
    use();
    switch(m_type)
    {
    case Type::INDEX:
        RenderBackEnd::shared()->resubmit(RenderFlag::BufferTarget::IndexBuffer, offset ,amount,data);
        break;
    case Type::VERTEX:
        RenderBackEnd::shared()->resubmit(RenderFlag::BufferTarget::VertexBuffer, offset ,amount,data);
        break;
    }
}

void RenderBuffer::use()
{
    switch(m_type)
    {
    case Type::INDEX:
        RenderBackEnd::shared()->bindBuffer(RenderFlag::BufferTarget::IndexBuffer,m_bufferId);
        break;
    case Type::VERTEX:
        RenderBackEnd::shared()->bindBuffer(RenderFlag::BufferTarget::VertexBuffer,m_bufferId);
        break;
    }
}
unsigned int RenderBuffer::bufferId() const
{
    return m_bufferId;
}

unsigned RenderBuffer::getAmount() const
{
	return m_amount;
}

RenderBuffer::~RenderBuffer()
{
	RenderBackEnd::shared()->deleteBuffer(m_bufferId);
}
} // namespace tzw

