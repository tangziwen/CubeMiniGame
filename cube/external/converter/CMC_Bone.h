#ifndef TZW_CMC_BONE_H
#define TZW_CMC_BONE_H

#include <vector>
#include <string>
#include <QMatrix4x4>
#include "CMC_BoneMetaInfo.h"
namespace tzw {

class CMC_Bone
{
public:
    CMC_Bone();

    CMC_BoneMetaInfo *info() const;
    void setInfo(CMC_BoneMetaInfo *info);
    void addChild(CMC_Bone * bone);
    std::vector <CMC_Bone * >m_children;
    QMatrix4x4 m_localTransform;// node to parent transform.
private:
    CMC_BoneMetaInfo * m_info;
};

} // namespace tzw

#endif // TZW_CMC_BONE_H
