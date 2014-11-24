#include "scene.h"
#include "external/SOIL/SOIL.h"
#include "renderer/pipeline.h"
#include "shader/shaderpoll.h"
#include "base/ogldev_math_3d.h"
#include "base/node.h"
Scene * Scene::currentScene = NULL;
Scene::Scene()
{
    initializeOpenGLFunctions();
    this->shadowMapFbo = new ShadowMapFBO();
    this->shadowMapFbo->Init(1024,768);
    this->setSkyBox(NULL);
    m_root.setName ("root");
}

void Scene::pushToRenderQueue(Entity *entity)
{
    this->m_entityList.push_back(entity);
}

void Scene::render()
{
    this->m_root.visit (this);
    if(!this->spotLights.empty ())
    {
        this->shadowPass(spotLights[0]);
    }
    this->shadowMapFbo->BindForReading(GL_TEXTURE1);
    this->renderPass();

}

DirectionalLight *Scene::getDirectionalLight()
{
    return & this->directionLight;
}

AmbientLight *Scene::getAmbientLight()
{
    return & this->ambientLight;
}

SpotLight *Scene::createSpotLight()
{
    SpotLight * spot_light = new SpotLight();
    this->spotLights.push_back(spot_light);
    return spot_light;
}

PointLight *Scene::createPointLight()
{
    PointLight * point_light = new PointLight();
    this->pointLights.push_back(point_light);
    return point_light;
}

void Scene::setAsCurrentScene()
{
    Scene::currentScene = this;
}



Scene *Scene::getCurrentScene()
{
    return Scene::currentScene;
}

SkyBox *Scene::skyBox() const
{
    return m_skyBox;
}

void Scene::setSkyBox(SkyBox *skyBox)
{
    m_skyBox = skyBox;
}

void Scene::setEntityBoneTransform(Entity *entity)
{
    if(entity->hasAnimation())
    {
        entity->getShaderProgram()->setUniformInteger("hasAnimation",1);
        std::vector <Matrix4f > transform;
        entity->bonesTransform(entity->animateTime(),transform,entity->animationName());
        for(int i =0;i<transform.size();i++)
        {
            char str[100];
            sprintf(str,"gBones[%d]",i);
            entity->getShaderProgram()->setUniformMat4v(str,(const GLfloat*)(transform[i].m),true);
        }
    }else{
        entity->getShaderProgram()->setUniformInteger("hasAnimation",0);
    }
}

void Scene::shadowPass(SpotLight * light)
{
    // Clear color and depth buffer
    this->shadowMapFbo->BindForWriting();
    glClear(GL_DEPTH_BUFFER_BIT);
    ShaderProgram * shadow_shader = ShaderPoll::getInstance ()->get("shadow");
    PipeLine p;
    for (std::vector<Entity *>::iterator i =m_entityList.begin();i!=m_entityList.end();i++)
    {
        shadow_shader->use ();
        Entity * entity = (* i);
        if(!entity->isEnableShadow()) continue;
        Camera * camera =entity->getCamera();
        p.setModelMatrix(entity->getModelTrans());
        p.setProjectionMatrix(camera->getProjection());
        QMatrix4x4 view;
        view.setToIdentity();
        view.lookAt(light->getPos(),light->getPos()+light->getDirection(),QVector3D(0,1,0));
        p.setViewMatrix(view);
        ShaderProgram * tmp_shader = entity->getShaderProgram ();
        entity->setShaderProgram (shadow_shader);
        setEntityBoneTransform(entity);
        p.apply(shadow_shader);
        entity->draw();
        entity->setShaderProgram (tmp_shader);
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);//switch the frame buffer back.
}



void Scene::renderPass()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    PipeLine p;
    for (std::vector<Entity *>::iterator i =m_entityList.begin();i!=m_entityList.end();i++)
    {
        Entity * entity = (* i);
        entity->getShaderProgram ()->use ();
        Camera * camera =entity->getCamera();
        p.setProjectionMatrix(camera->getProjection());
        p.setViewMatrix(camera->getViewMatrix());


        QMatrix4x4 lightView;
        lightView.setToIdentity();
        lightView.lookAt(spotLights[0]->getPos(),this->spotLights[0]->getPos()+this->spotLights[0]->getDirection(),QVector3D(0,1,0));
        p.setLightViewMatrix(lightView);

        p.setModelMatrix(entity->getModelTrans());
        p.applyLightMvp(entity->getShaderProgram());
        this->shadowMapFbo->applyShadowMapTexture(entity->getShaderProgram());
        calculateLight(entity->getShaderProgram());
        setEntityBoneTransform(entity);
        p.apply(entity->getShaderProgram());
        entity->draw();
        if(entity->onRender)
        {
            entity->onRender(entity,0);
        }
    }
    if(m_skyBox)
    {
        Camera * camera =m_skyBox->camera();
        m_skyBox->shader()->use();
        p.setProjectionMatrix(camera->getProjection());
        p.setViewMatrix(camera->getViewMatrix());
        m_skyBox->getEntity()->translate(camera->pos ().x (),camera->pos ().y (),camera->pos ().z ());
        p.setModelMatrix(m_skyBox->getEntity()->getModelTrans());
        p.apply(m_skyBox->shader());
        m_skyBox->Draw();
    }
    this->m_entityList.clear ();
}

void Scene::calculateLight(ShaderProgram *shader)
{
    shader->setUniformInteger("pointLightAmount",this->pointLights.size());
    shader->setUniformInteger("spotLightAmount",this->spotLights.size());
    this->directionLight.apply(shader);
    this->ambientLight.apply(shader);
    for(int j = 0;j<this->pointLights.size();j++)
    {
        this->pointLights[j]->apply(shader,j);
    }

    for(int k = 0;k<this->spotLights.size();k++)
    {
        this->spotLights[k]->apply(shader,k);
    }
}

Node *Scene::root()
{
    return &m_root;
}






