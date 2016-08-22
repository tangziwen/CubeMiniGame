#ifndef TZW_RENDERTARGET_H
#define TZW_RENDERTARGET_H
#include "../Engine/EngineDef.h"
#include <vector>
namespace tzw {

class RenderTarget
{
public:
    RenderTarget();
    void init(integer_u width, integer_u height, integer_u numOfOutputs = 4, bool isUseDepth = true);
    void bindForWriting();
    void bindForReading();
    void setReadBuffer(integer_u index);
private:
    integer_u m_width;
    integer_u m_height;
    integer_u m_fbo;
    integer_u * m_colorTexs;
    integer_u m_depthTex;
    bool m_isUseDepth;
    integer_u m_numOfOutputs;
};

} // namespace tzw

#endif // TZW_RENDERTARGET_H
