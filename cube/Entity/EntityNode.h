#ifndef SUBENTITY_H
#define SUBENTITY_H
#include "base/node.h"
#include "geometry/mesh.h"
#include <vector>
#include "external/converter/CMC_Node.h"
// Entity node is the bone or the node of the entity(model), which may construced by artist via some model Editor or scene Editor.
class EntityNode : public Node
{
public:
    EntityNode();
    EntityNode * findNode(std::string nodeName);
    tzw::CMC_Node *nodeData() const;
    void setNodeData(tzw::CMC_Node *nodeData);

private:
    tzw::CMC_Node * m_nodeData;
    EntityNode * findNodeRecursively(EntityNode * node,std::string nodeName);
};

#endif // SUBENTITY_H
