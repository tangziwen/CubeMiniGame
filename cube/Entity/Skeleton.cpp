#include "Skeleton.h"
#include "Entity.h"
Skeleton::Skeleton(Entity *entity, int boneSize)
    :m_entityNodeRoot(nullptr)
{
    m_entity = entity;
    m_bonePalette.resize (boneSize);
}
EntityNode *Skeleton::getEntityNodeRoot() const
{
    return m_entityNodeRoot;
}

void Skeleton::setEntityNodeRoot(EntityNode *entityNodeRoot)
{
    m_entityNodeRoot = entityNodeRoot;
}

void Skeleton::updateNodeAndBone(tzw::CMC_AnimateData *animateData, float percentage)
{
    QMatrix4x4 mat;
    mat.setToIdentity ();
    updateNodesAndBonesRecursively(m_entityNodeRoot,mat,animateData,percentage);
}

void Skeleton::updateNodesAndBonesRecursively(EntityNode *theNode, QMatrix4x4 parentTransform, tzw::CMC_AnimateData *animateData, float percentage)
{
    auto sub_entity = theNode;
    std::string NodeName = sub_entity->name ();
    auto node = sub_entity->nodeData ();
    QMatrix4x4 NodeTransform;
    QMatrix4x4 globalTransform;
    if(m_entity->getModelData ()->isBone (NodeName))
    {
        //do animation;
        float t = percentage;
        const tzw::CMC_AnimateBone * pNodeAnim = animateData->findAnimateBone (NodeName);
        // Interpolate scaling and generate scaling transformation matrix
        QVector3D Scaling;
        pNodeAnim->calcInterpolatedScaling (Scaling,t);
        QMatrix4x4 ScalingM;
        ScalingM.setToIdentity ();
        ScalingM.scale (Scaling.x (),Scaling.y (),Scaling.z ());

        // Interpolate rotation and generate rotation transformation matrix
        QQuaternion RotationQ;
        pNodeAnim->calcInterpolatedRotation (RotationQ,t);
        QMatrix4x4 RotationM;
        RotationM.setToIdentity ();
        RotationM.rotate(RotationQ);

        // Interpolate translation and generate translation transformation matrix
        QVector3D Translation;
        pNodeAnim->calcInterpolatedPosition (Translation,t);
        QMatrix4x4 TranslationM;
        TranslationM.setToIdentity ();
        TranslationM.translate (Translation.x(),Translation.y (),Translation.z ());

        auto offsetMatrix = node->info ()->defaultBoneOffset ();
        // Combine the above transformations
        NodeTransform = TranslationM*RotationM*ScalingM; //t * r *s

        //hierarchically bone transform.
        globalTransform = parentTransform * NodeTransform;

        theNode->setGlobalBoneTransform (globalTransform);

        //write to palette.
        uint BoneIndex = m_entity->getModelData ()->m_BoneMetaInfoMapping[NodeName];
        m_bonePalette[BoneIndex] = m_entity->getModelData ()->globalInverseTransform ()* globalTransform * offsetMatrix;

    }else
    {
        NodeTransform = node->m_localTransform;
        globalTransform = parentTransform * NodeTransform;
    }
    for(int i =0;i<sub_entity->getChildrenAmount ();i++)
    {
        auto child = sub_entity->getChild (i);
        if(child->nodeType ()!=NODE_TYPE_BONE) continue;
        updateNodesAndBonesRecursively((EntityNode *)sub_entity->getChild (i),globalTransform,animateData,percentage);
    }
}

QMatrix4x4 Skeleton::getMatrixFromBonePalatte(int index)
{
    return m_bonePalette[index];
}

void Skeleton::copyBonePalette(std::vector<QMatrix4x4> &Transforms)
{
    if(!m_entity->hasAnimation ())
    {
        Transforms.clear ();
        return;
    }
    int numOfBones = m_entity->getModelData ()->m_boneMetaInfoList.size ();
    Transforms.resize(numOfBones);
    for (uint i = 0 ; i < numOfBones ; i++) {
        Transforms[i] = m_bonePalette[i];
    }
}


