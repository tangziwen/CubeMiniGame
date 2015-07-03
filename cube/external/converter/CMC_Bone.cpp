#include "CMC_Bone.h"

namespace tzw {

CMC_Bone::CMC_Bone()
{
    m_info = nullptr;
}
CMC_BoneMetaInfo *CMC_Bone::info() const
{
    return m_info;
}

void CMC_Bone::setInfo(CMC_BoneMetaInfo *info)
{
    m_info = info;
}

void CMC_Bone::addChild(CMC_Bone *bone)
{
    m_children.push_back (bone);
}




} // namespace tzw

