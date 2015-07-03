#ifndef TZW_CMC_ANIMATE_H
#define TZW_CMC_ANIMATE_H
#include "CMC_AnimateBone.h"

namespace tzw {

class CMC_Animate
{
public:
    CMC_Animate();
    std::vector<CMC_AnimateBone *>m_animateBone;
    void addAnimateBone(CMC_AnimateBone * bone);
    float m_ticksPerSecond;
};

} // namespace tzw

#endif // TZW_CMC_ANIMATE_H
