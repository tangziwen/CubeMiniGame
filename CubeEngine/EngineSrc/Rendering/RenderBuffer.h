#ifndef TZW_RENDERBUFFER_H
#define TZW_RENDERBUFFER_H


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
    void allocate(void * data, unsigned int amount);
    void use();
    unsigned int bufferId() const;

private:
    Type m_type;
    unsigned int m_bufferId;
};

} // namespace tzw

#endif // TZW_RENDERBUFFER_H
