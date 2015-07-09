#include "CMC_BoneMetaInfo.h"

namespace tzw {

CMC_BoneMetaInfo::CMC_BoneMetaInfo()
{

}
std::string CMC_BoneMetaInfo::name() const
{
    return m_name;
}

void CMC_BoneMetaInfo::setName(const std::string &name)
{
    m_name = name;
}
QMatrix4x4 CMC_BoneMetaInfo::defaultOffset() const
{
    return m_defaultOffset;
}

void CMC_BoneMetaInfo::setDefaultOffset(const QMatrix4x4 &defaultOffset)
{
    m_defaultOffset = defaultOffset;
}



} // namespace tzw

