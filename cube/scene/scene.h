#ifndef SCENE_H
#define SCENE_H
#include "Entity/entity.h"
#include <vector>
#include "light/directionallight.h"
#include "light/ambientlight.h"
#include "light/pointlight.h"
#include "light/spotlight.h"
#include <qopenglfunctions.h>
#include "light/shadow_map_fbo.h"
#include "Entity/skybox.h"
class Node;
class Scene : protected QOpenGLFunctions
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
private:
    Node  m_root;
    SkyBox * m_skyBox;
    void setEntityBoneTransform(Entity *entity);
    ShadowMapFBO * shadowMapFbo;
    void shadowPass(SpotLight *light);
    void renderPass();
    void calculateLight(ShaderProgram * shader);
    static Scene * currentScene;
    std::vector<Entity *> m_entityList;
    DirectionalLight directionLight;
    AmbientLight ambientLight;
    PointLight pointLight;
    SpotLight spotLight;
    std::vector<PointLight * > pointLights;
    std::vector <SpotLight * > spotLights;
};

#endif // SCENE_H
