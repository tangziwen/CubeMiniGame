#include "EntityNode.h"

EntityNode::EntityNode()
    :m_nodeData(nullptr)
{
    this->m_nodeType = NODE_TYPE_BONE;
}

EntityNode *EntityNode::findNode(std::string nodeName)
{
    return findNodeRecursively(this,nodeName);
}
tzw::CMC_Node *EntityNode::nodeData() const
{
    return m_nodeData;
}

void EntityNode::setNodeData(tzw::CMC_Node *nodeData)
{
    m_nodeData = nodeData;
}


EntityNode *EntityNode::findNodeRecursively(EntityNode *node, std::string nodeName)
{
    if(std::string(node->name()) == nodeName)
    {
        return node;
    }
    for(int i =0;i<node->getChildrenAmount ();i++)
    {
        auto child = node->getChild (i);
        if(child->nodeType ()!=NODE_TYPE_BONE) continue;
        auto result = findNodeRecursively ((EntityNode *)child,nodeName);
        if(result)
        {
            return result;
        }
    }
    return nullptr;
}


