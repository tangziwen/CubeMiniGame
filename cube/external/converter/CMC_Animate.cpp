#include "CMC_Animate.h"

namespace tzw {

CMC_Animate::CMC_Animate()
{

}

void CMC_Animate::addAnimateBone(CMC_AnimateBone *bone)
{
    m_animateBone.push_back (bone);
}

} // namespace tzw

