#ifndef SUBENTITY_H
#define SUBENTITY_H
#include "base/node.h"
#include "geometry/mesh.h"
#include <vector>
#include "external/converter/CMC_Node.h"
class Entity;
// Entity node is the bone or the node of the entity(model), which may construced by artist via some model Editor or scene Editor.
class EntityNode : public Node
{
public:
    EntityNode(Entity * entity);
    EntityNode * findNode(std::string nodeName);
    tzw::CMC_Node *nodeData() const;
    void setNodeData(tzw::CMC_Node *nodeData);
    virtual QMatrix4x4 getModelTrans();
    QMatrix4x4 getGlobalBoneTransform() const;
    void setGlobalBoneTransform(const QMatrix4x4 &globalTransform);
private:
    QMatrix4x4 m_globalBoneTransform;
    Entity * m_entity;
    tzw::CMC_Node * m_nodeData;
    EntityNode * findNodeRecursively(EntityNode * node,std::string nodeName);
};

#endif // SUBENTITY_H
