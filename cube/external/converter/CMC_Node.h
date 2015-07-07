#ifndef TZW_CMC_BONE_H
#define TZW_CMC_BONE_H

#include <vector>
#include <string>
#include <QMatrix4x4>
#include "CMC_NodeMetaInfo.h"
namespace tzw {

class CMC_Node
{
public:
    CMC_Node();
    CMC_NodeMetaInfo *info() const;
    void setInfo(CMC_NodeMetaInfo *info);
    void addChild(CMC_Node * bone);
    std::vector <CMC_Node * >m_children;
    QMatrix4x4 m_localTransform;// node to parent transform.
    CMC_Node *parent() const;
    void setParent(CMC_Node *parent);
private:
    CMC_NodeMetaInfo * m_info;
    CMC_Node *m_parent;
};
} // namespace tzw

#endif // TZW_CMC_BONE_H
