#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <QMatrix4x4>
#include <QOpenGLFunctions_3_0>

#include "Entity/entity.h"
#include "GUI/sprite.h"

#include "light/directionallight.h"
#include "light/ambientlight.h"
#include "light/pointlight.h"
#include "light/spotlight.h"

#include "light/shadow_map_fbo.h"
#include "Entity/skybox.h"
#include "renderer/gbuffer.h"
#include "renderer/renderbuffer.h"
#include "base/RenderTarget.h"


#define DEFERRED_SHADING 1
#define FORWARD_SHADING 0
class Node;
class Scene : protected QOpenGLFunctions_3_0
{
public:
    Scene();
    void pushEntityToRenderQueue(Entity * entity);
    void pushSpriteToRenderQueue(Sprite * sprite);
    void pushCustomNodeToRenderQueue(Node * node);
    void render();
    DirectionalLight *getDirectionalLight();
    AmbientLight * getAmbientLight();
    SpotLight * createSpotLight();
    PointLight * createPointLight();
    void setAsCurrentScene();
    static Scene * getCurrentScene();
    SkyBox *skyBox() const;
    void setSkyBox(SkyBox *skyBox);
    Node * root();
    int renderType() const;
    void setRenderType(int renderType);
    Camera *camera() const;
    void setCamera(Camera *camera);
    Camera *guiCamera() const;
    void setGuiCamera(Camera *guiCamera);
    std::vector<Entity *> getPotentialEntityList() const;
    void addRenderTarget(RenderTarget * target);

private:
    void sortRenderQue();
    void pickBright();
    void gaussianBlur_H(float size);
    void gaussianBlur_V(float size);
    void linearBlur(float radius,float samples);
    void setEntityBoneTransform(Entity *entity);
    void spriteRenderPass();
    void customNodeRenderPass();
    void shadowPassForSpot(SpotLight *light, RenderTarget *target);
    void shadowPassDirectional(RenderTarget *target);

    //geometryPass : grab all info including diffuse,Normal,depth etc. to one G-Buffer.
    void geometryPass(RenderTarget *target);

    void lightPass(RenderTarget * target);
    void spotLightPass(RenderTarget *target);
    void pointLightPass(RenderTarget * target);
    void directionLightPass(RenderTarget * target);
    void calculateLight(ShaderProgram * shader);
    void deferredRendering(RenderTarget * target);
    void forwardRendering();
    QMatrix4x4 getCropMatrix(AABB frustumAABB);
private:
    RenderTarget * m_mainRenderTarget;
    Camera * m_guiCamera;
    Entity * m_quad;
    GBuffer * m_GBuffer;
    RenderBuffer * bloom_fbo1;
    RenderBuffer * bloom_fbo2;
    RenderBuffer * bloom_fbo3;
    int m_renderType;
    Node  m_root;
    SkyBox * m_skyBox;
    ShadowMapFBO * spotLightshadowMapFbo;
    ShadowMapFBO * directionalLightshadowMapFbo;
    static Scene * currentScene;
    std::vector<Entity *> m_entityList;
    std::vector<Entity *> m_tempEntityList;
    std::vector<Sprite *>m_spriteList;
    std::vector<Node*> m_customNodeList;
    std::vector<RenderTarget*> m_renderTargetList;
    DirectionalLight directionLight;
    AmbientLight ambientLight;
    PointLight pointLight;
    SpotLight spotLight;
    std::vector<PointLight * > pointLights;
    std::vector <SpotLight * > spotLights;
};

#endif // SCENE_H
