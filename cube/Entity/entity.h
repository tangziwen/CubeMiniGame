#ifndef ENTITY_H
#define ENTITY_H
#include <QVector3D>
#include <vector>
#include <QMatrix4x4>
#include <QGLFunctions>
#include <QGLShaderProgram>
#include <QMatrix4x4>
#include <functional>
#include "geometry/mesh.h"
#include "renderer/meshdrawcomand.h"
#include "shader/shaderpoll.h"
#include "base/camera.h"
#include "material/material.h"
#include "base/bonedata.h"
#include "base/boneinfo.h"
#include "base/ogldev_math_3d.h"
#include "external/include/assimp/scene.h"
#include "external/include/assimp/Importer.hpp"
#include "external/include/assimp/postprocess.h"
#include "base/node.h"

class Entity : public Node
{
public:
    Entity();
    Entity(const char * file_name);
    void addMesh(TMesh *mesh);
    TMesh * getMesh(int index);
    void draw(bool withoutexture =false);
    void setShaderProgram(ShaderProgram * program);
    void setCamera(Camera * camera);
    Camera * getCamera();
    ShaderProgram * getShaderProgram();
    void bonesTransform(float TimeInSeconds, std::vector<Matrix4f> &Transforms, std::string animation_name);
    void animate(float time,const char * animation_name);
    float animateTime() const;
    void setAnimateTime(float animateTime);
    std::string animationName() const;
    void setAnimationName(const std::string &animationName);
    bool hasAnimation() const;
    void setHasAnimation(bool hasAnimation);
    bool isEnableShadow() const;
    void setIsEnableShadow(bool isEnableShadow);
    void (*onRender)(Entity * self,float dt);
    AABB getAABB();
    float getDistToCamera();
private:
    uint findBoneInterpoScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint findBoneInterpoRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint findBoneInterpoTranslation(float AnimationTime, const aiNodeAnim* pNodeAnim);
    const aiNodeAnim* findNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
    void readNodeHeirarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform);
    void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void loadModelData(const char * file_name);
    void LoadMaterial(const aiScene* pScene, const char *file_name,const char * pre_fix);
    void loadBones(const aiMesh* pMesh, TMesh *mesh);
private:
    bool m_isEnableShadow;
    bool m_hasAnimation;
    float m_animateTime;
    std::string m_animationName;
    ShaderProgram * program;
    std::vector <TMesh *> mesh_list;
    Camera *camera;
    std::vector <Material * >material_list;
    Matrix4f m_globalInverseTransform;
    std::map<std::string,int> m_BoneMapping;  // maps a bone name to its index
    int m_numBones;
    std::vector<BoneInfo> m_BoneInfo;
    aiScene* m_pScene;
    Assimp::Importer m_Importer;
    AABB m_aabb;
    bool m_isAABBDirty;
};

#endif // ENTITY_H
