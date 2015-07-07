#include "CMC_NodeMetaInfo.h"

namespace tzw {

CMC_NodeMetaInfo::CMC_NodeMetaInfo()
{

}
std::string CMC_NodeMetaInfo::name() const
{
    return m_name;
}

void CMC_NodeMetaInfo::setName(const std::string &name)
{
    m_name = name;
}
QMatrix4x4 CMC_NodeMetaInfo::defaultBoneOffset() const
{
    return m_defaultBoneOffset;
}

void CMC_NodeMetaInfo::setDefaultBoneOffset(const QMatrix4x4 &defaultOffset)
{
    m_defaultBoneOffset = defaultOffset;
}



} // namespace tzw

