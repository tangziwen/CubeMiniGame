#ifndef TZW_CMC_ANIMATE_H
#define TZW_CMC_ANIMATE_H
#include "CMC_AnimateBone.h"

namespace tzw {

class CMC_AnimateData
{
public:
    CMC_AnimateData();
    std::vector<CMC_AnimateBone *>m_animateBone;
    void addAnimateBone(CMC_AnimateBone * bone);
    CMC_AnimateBone * findAnimateBone(std::string boneName);
    float m_ticksPerSecond;
    float m_duration;
    std::string m_name;
};

} // namespace tzw

#endif // TZW_CMC_ANIMATE_H
