#include "CMC_Node.h"

namespace tzw {

CMC_Node::CMC_Node()
    :m_parent(nullptr),m_info(nullptr)
{

}
CMC_NodeMetaInfo *CMC_Node::info() const
{
    return m_info;
}

void CMC_Node::setInfo(CMC_NodeMetaInfo *info)
{
    m_info = info;
}

void CMC_Node::addChild(CMC_Node *bone)
{
    bone->setParent (this);
    m_children.push_back (bone);
}
CMC_Node *CMC_Node::parent() const
{
    return m_parent;
}

void CMC_Node::setParent(CMC_Node *parent)
{
    m_parent = parent;
}

QMatrix4x4 CMC_Node::getOffset()
{
    if(m_parent)
    {
        return m_parent->getOffset ()*m_localTransform;
    }else
    {
        return m_localTransform;
    }
}


} // namespace tzw

