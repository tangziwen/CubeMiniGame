#ifndef TZW_RENDERTARGET_H
#define TZW_RENDERTARGET_H
#include "../Engine/EngineDef.h"
#include <vector>
#include "Math/vec2.h"

namespace tzw {

class FrameBuffer
{
public:
    FrameBuffer();
    void init(integer_u width, integer_u height, integer_u numOfOutputs = 4, bool isUseDepth = true, bool isHDRCompatible = false);
    void bindForWriting();
    void bindForReadingGBuffer();
    void bindForReading();
    void bindDepth(int index);
    void setReadBuffer(integer_u index);
	void bindRtToTexture(integer_u gbufferID, integer_u index);
	vec2 getFrameSize() const;
	void gen();
	void setIsLinearFilter(bool isUseLinear);
	bool getIsLinearFilter() const;
private:
    integer_u m_width;
    integer_u m_height;
    integer_u m_fbo;
    integer_u * m_colorTexs;
    integer_u m_depthTex;
    bool m_isUseDepth;
	bool m_isHDRCompatible;
	bool m_isLinearFilter;
    integer_u m_numOfOutputs;
};

} // namespace tzw

#endif // TZW_RENDERTARGET_H
