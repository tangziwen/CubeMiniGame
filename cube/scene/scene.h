#ifndef SCENE_H
#define SCENE_H
#include "Entity/entity.h"
#include <vector>
#include "light/directionallight.h"
#include "light/ambientlight.h"
#include "light/pointlight.h"
#include "light/spotlight.h"
#include <QOpenGLFunctions_3_0>
#include "light/shadow_map_fbo.h"
#include "Entity/skybox.h"
#include "renderer/gbuffer.h"

#define DEFERRED_SHADING 1
#define FORWARD_SHADING 0
class Node;
class Scene : protected QOpenGLFunctions_3_0
{
public:
    Scene();
    void pushToRenderQueue(Entity * entity);
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

private:
    Camera * m_camera;
    Entity * m_quad;
    Entity * m_sphere;
    GBuffer * m_GBuffer;
    int m_renderType;
    Node  m_root;
    SkyBox * m_skyBox;
    void setEntityBoneTransform(Entity *entity);
    ShadowMapFBO * shadowMapFbo;
    void shadowPass(SpotLight *light);
    void renderPass();
    void geometryPass();
    void lightPass();
    void spotLightPass();
    void pointLightPass();
    void directionLightPass();
    void calculateLight(ShaderProgram * shader);
    static Scene * currentScene;
    std::vector<Entity *> m_entityList;
    DirectionalLight directionLight;
    AmbientLight ambientLight;
    PointLight pointLight;
    SpotLight spotLight;
    std::vector<PointLight * > pointLights;
    std::vector <SpotLight * > spotLights;
    void deferredRendering();
    void forwardRendering();
};

#endif // SCENE_H
