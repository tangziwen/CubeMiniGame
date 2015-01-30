#ifndef SCENE_H
#define SCENE_H
#include "Entity/entity.h"
#include "GUI/sprite.h"
#include <vector>
#include "light/directionallight.h"
#include "light/ambientlight.h"
#include "light/pointlight.h"
#include "light/spotlight.h"
#include <QOpenGLFunctions_3_0>
#include "light/shadow_map_fbo.h"
#include "Entity/skybox.h"
#include "renderer/gbuffer.h"
#include "renderer/renderbuffer.h"
#define DEFERRED_SHADING 1
#define FORWARD_SHADING 0
class Node;
class Scene : protected QOpenGLFunctions_3_0
{
public:
    Scene();
    void pushEntityToRenderQueue(Entity * entity);
    void pushSpriteToRenderQueue(Sprite * sprite);
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

private:
    void sortRenderQue();
    void pickBright();
    void gaussianBlur_H(float size);
    void gaussianBlur_V(float size);
    void linearBlur(float radius,float samples);
    void setEntityBoneTransform(Entity *entity);
    void spriteRenderPass();
    void shadowPass(SpotLight *light);
    void forwardRenderPass();
    void geometryPass();
    void lightPass();
    void spotLightPass();
    void pointLightPass();
    void directionLightPass();
    void calculateLight(ShaderProgram * shader);
    void deferredRendering();
    void forwardRendering();

private:
    Camera * m_guiCamera;
    Camera * m_camera;
    Entity * m_quad;
    Entity * m_sphere;
    GBuffer * m_GBuffer;
    RenderBuffer * bloom_fbo1;
    RenderBuffer * bloom_fbo2;
    RenderBuffer * bloom_fbo3;
    int m_renderType;
    Node  m_root;
    SkyBox * m_skyBox;
    ShadowMapFBO * shadowMapFbo;
    static Scene * currentScene;
    std::vector<Entity *> m_entityList;
    std::vector<Entity *> m_tempEntityList;
    std::vector<Sprite *>m_spriteList;
    DirectionalLight directionLight;
    AmbientLight ambientLight;
    PointLight pointLight;
    SpotLight spotLight;
    std::vector<PointLight * > pointLights;
    std::vector <SpotLight * > spotLights;
};

#endif // SCENE_H
