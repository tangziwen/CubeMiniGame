#include "EntityNode.h"
#include "Entity.h"
#include "external/converter/CMC_Node.h"
EntityNode::EntityNode(Entity *entity)
    :m_nodeData(nullptr),m_entity(entity)
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

QMatrix4x4 EntityNode::getModelTrans()
{
    QMatrix4x4 mat = m_entity->getModelData ()->globalInverseTransform ()* getGlobalBoneTransform ();
    return m_entity->getModelTrans () * mat;
}
QMatrix4x4 EntityNode::getGlobalBoneTransform() const
{
    return m_globalBoneTransform;
}

void EntityNode::setGlobalBoneTransform(const QMatrix4x4 &globalTransform)
{
    m_globalBoneTransform = globalTransform;
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


