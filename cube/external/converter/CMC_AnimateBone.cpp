#include "CMC_AnimateBone.h"

namespace tzw {

CMC_AnimateBone::CMC_AnimateBone()
{

}

void CMC_AnimateBone::addTranslate(CMC_TranslateKey v)
{
    m_transKeys.push_back (v);
}

void CMC_AnimateBone::addScale(CMC_ScaleKey v)
{
    m_scaleKeys.push_back (v);
}

void CMC_AnimateBone::addRotate(CMC_RotateKey q)
{
    m_rotateKeys.push_back (q);
}

} // namespace tzw

