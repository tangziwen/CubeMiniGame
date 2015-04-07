#ifndef OCTREE_H
#define OCTREE_H


#include <QVector3D>
#include <vector>

#include "Entity/entity.h"
#include "geometry/aabb.h"
#include "base/frustum.h"

#define OCTREE_MAX_LEVEL 4
struct OCTreeNode
{
public:
    OCTreeNode(QVector3D orgin,QVector3D size,OCTreeNode * parent);
    bool addEntity(Entity * entity);
    int getHierarchyLevel();
    OCTreeNode *parent() const;
    void setParent(OCTreeNode *parent);
    void visit(Frustum & frustum,std::vector<Entity *> * entity_list);
private:
    std::vector<Entity *> m_entityList;
    OCTreeNode * m_children[8];
    OCTreeNode * m_parent;
    AABB m_aabb;
    QVector3D m_origin;
    QVector3D m_size;
};

class OCTree
{
public:
    void init(float x, float y, float z);
    void init(QVector3D size);
    void visitByFrustm(Frustum & frustum,std::vector<Entity *> * entity_list);
private:
    QVector3D m_size;
    OCTreeNode * m_root;
};

#endif // OCTREE_H
