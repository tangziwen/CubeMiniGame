#ifndef TZW_RENDERBUFFER_H
#define TZW_RENDERBUFFER_H

#include "RenderFlag.h"
namespace tzw {

class RenderBuffer
{

public:
    enum class Type{
        VERTEX,
        INDEX
    };

    RenderBuffer(Type bufferType);
    void create();
    void allocate(void * data, unsigned int amount, RenderFlag::BufferStorageType storageType = RenderFlag::BufferStorageType::STATIC_DRAW);
	void resubmit(void * data, unsigned int offset, unsigned int amount);
    void use();
    unsigned int bufferId() const;
	unsigned getAmount() const;
private:
    Type m_type;
    unsigned int m_bufferId;
	unsigned int m_amount;
	
};

} // namespace tzw

#endif // TZW_RENDERBUFFER_H
