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
#include "external/converter/CMC_ModelData.h"
#include "base/node.h"

#include "Skeleton.h"
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

    int getCurrentAnimateIndex() const;
    void setCurrentAnimateIndex(int currentAnimateIndex);

    tzw::CMC_ModelData *getModelData() const;
    void setModelData(tzw::CMC_ModelData *modelData);

    Skeleton *getSkeleton() const;
    void setSkeleton(Skeleton *skeleton);
    float getPercentageOfAnimate();

private:
    void loadModelData(tzw::CMC_ModelData *cmc_model, const char *file_name);
    void loadMaterial(tzw::CMC_ModelData * model, const char * file_name,const char * pre_fix);
    Texture *loadTextureFromMaterial(std::string fileName, const char * pre_fix);
    void createNode(Node *parent, tzw::CMC_Node *node);

private:
    Skeleton * m_skeleton;
    tzw::CMC_ModelData * m_modelData;
    bool m_isEnableShadow;
    bool m_hasAnimation;
    float m_animateTime;
    ShaderProgram * program;
    std::vector <TMesh *> mesh_list;
    std::vector <Material * >material_list;
    Matrix4f m_globalInverseTransform;
    std::vector<QMatrix4x4> m_bonePalette;
    AABB m_aabb;
    bool m_isAABBDirty;
    int m_currentAnimateIndex;
protected:
    Camera *camera;
};


#endif // ENTITY_H
