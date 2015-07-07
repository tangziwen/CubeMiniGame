#include "CMC_AnimateData.h"

namespace tzw {

CMC_AnimateData::CMC_AnimateData()
{

}

void CMC_AnimateData::addAnimateBone(CMC_AnimateBone *bone)
{
    m_animateBone.push_back (bone);
}

CMC_AnimateBone *CMC_AnimateData::findAnimateBone(std::string boneName)
{
    for(int i =0;i<m_animateBone.size ();i++)
    {
        CMC_AnimateBone * animateBone = m_animateBone[i];
        if(animateBone->m_boneName.compare (boneName)==0)
        {
            return animateBone;
        }
    }
    return nullptr;
}

} // namespace tzw

