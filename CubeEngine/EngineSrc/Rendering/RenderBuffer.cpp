#include "RenderBuffer.h"
#include "../BackEnd/RenderBackEnd.h"
namespace tzw {

RenderBuffer::RenderBuffer(Type bufferType)
    :m_type(bufferType),m_bufferId(-1)
{

}

void RenderBuffer::create()
{
    m_bufferId = RenderBackEnd::shared()->genBuffer();
}

void RenderBuffer::allocate(void *data, unsigned int amount)
{
    use();
    switch(m_type)
    {
    case Type::INDEX:
        RenderBackEnd::shared()->uploadBufferData(RenderFlag::BufferTarget::IndexBuffer,amount,data);
        break;
    case Type::VERTEX:
        RenderBackEnd::shared()->uploadBufferData(RenderFlag::BufferTarget::VertexBuffer,amount,data);
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


} // namespace tzw

