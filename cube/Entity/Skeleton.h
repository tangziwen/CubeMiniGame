#ifndef SKELETON_H
#define SKELETON_H
#include "EntityNode.h"
#include "external/converter/CMC_AnimateData.h"
class Skeleton
{
public:
    Skeleton(Entity * entity,int boneSize);

    EntityNode *getEntityNodeRoot() const;
    void setEntityNodeRoot(EntityNode *getEntityNodeRoot);

    void updateNodeAndBone(tzw::CMC_AnimateData * animateData,float percentage);

    QMatrix4x4 getMatrixFromBonePalatte(int index);
    void copyBonePalette(std::vector<QMatrix4x4> &Transforms);
private:
    EntityNode * m_entityNodeRoot;
    Entity * m_entity;
    std::vector<QMatrix4x4> m_bonePalette;
    void updateNodesAndBonesRecursively(EntityNode * theNode, QMatrix4x4 parentTransform,tzw::CMC_AnimateData * animateData,float percentage);
};

#endif // SKELETON_H
