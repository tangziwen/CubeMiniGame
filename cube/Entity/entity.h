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
#include "shader/ShaderPool.h"
#include "base/camera.h"
#include "material/material.h"
#include "base/bonedata.h"
#include "base/boneinfo.h"
#include "base/ogldev_math_3d.h"
#include "external/include/assimp/scene.h"
#include "external/include/assimp/Importer.hpp"
#include "external/include/assimp/postprocess.h"
#include "external/converter/CMC_ModelData.h"
#include "base/node.h"
#include "EntityNode.h"
class Entity : public Node
{
public:
    enum class LoadPolicy
    {
        LoadFromLoader,
        LoadFromTzw,
    };

    Entity();
    Entity(const char * file_name,LoadPolicy policy = LoadPolicy::LoadFromLoader);
    void addMesh(TMesh *mesh);
    TMesh * getMesh(int index);
    void draw(bool withoutexture =false);
    void setShaderProgram(ShaderProgram * program);
    void setCamera(Camera * camera);
    Camera * getCamera();
    ShaderProgram * getShaderProgram();
    void copyBonePalette(std::vector<QMatrix4x4> &Transforms);
    void playAnimate(int index, float time);
    float animateTime() const;
    void setAnimateTime(float animateTime);
    bool hasAnimation() const;
    void setHasAnimation(bool hasAnimation);
    bool isEnableShadow() const;
    void setIsEnableShadow(bool isEnableShadow);
    std::function<void (Entity * self, float dt)> onRender;
    AABB getAABB();
    float getDistToCamera();
    virtual void adjustVertices();
    bool isSetDrawWire() const;
    void setIsSetDrawWire(bool isSetDrawWire);

    void updateNodeAndBone();
    EntityNode *getEntityNodeRoot() const;
    void setEntityNodeRoot(EntityNode *entityNodeRoot);

    int getCurrentAnimateIndex() const;
    void setCurrentAnimateIndex(int currentAnimateIndex);

    tzw::CMC_ModelData *getModelData() const;
    void setModelData(tzw::CMC_ModelData *modelData);

private:
    void loadModelDataFromTZW(tzw::CMC_ModelData *cmc_model, const char *file_name);
    void loadMaterialFromTZW(tzw::CMC_ModelData * model, const char * file_name,const char * pre_fix);
    Texture *loadTextureFromMaterial(std::string fileName, const char * pre_fix);
    void createNode(Node *parent, tzw::CMC_Node *node);
    void updateNodesAndBonesRecursively(EntityNode * theNode, QMatrix4x4 parentTransform);
    float getPercentageOfAnimate();

private:
    tzw::CMC_ModelData * m_modelData;
    EntityNode * m_entityNodeRoot;
    bool m_isSetDrawWire;
    bool m_isEnableShadow;
    bool m_hasAnimation;
    float m_animateTime;
    ShaderProgram * program;
    std::vector <TMesh *> mesh_list;
    std::vector <Material * >material_list;
    Matrix4f m_globalInverseTransform;
    std::map<std::string,int> m_BoneMapping;  // maps a bone name to its index
    int m_numBones;
    std::vector<BoneInfo> m_BoneInfo;
    std::vector<QMatrix4x4> m_bonePalette;
    Assimp::Importer m_Importer;
    AABB m_aabb;
    bool m_isAABBDirty;
    int m_currentAnimateIndex;
protected:
    Camera *camera;
};

#endif // ENTITY_H
