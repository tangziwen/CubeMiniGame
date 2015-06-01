#include <algorithm>
#include "scene.h"
#include "external/SOIL/SOIL.h"
#include "renderer/pipeline.h"
#include "shader/shaderpoll.h"
#include "base/ogldev_math_3d.h"
#include "base/node.h"
#include "geometry/mesh.h"
#include "material/materialpool.h"

#include <QMessageBox>
Scene * Scene::currentScene = NULL;
Scene::Scene()
{
    initializeOpenGLFunctions();

    this->setSkyBox(NULL);
    m_root.setName ("root");
    this->setRenderType (DEFERRED_SHADING);
    m_mainRenderTarget = new RenderTarget();
    m_mainRenderTarget->setType (RenderTarget::TargetType::ON_SCREEN);
    bloom_fbo1 = new RenderBuffer(1024,768);
    bloom_fbo2 = new RenderBuffer(1024,768);
    bloom_fbo3 = new RenderBuffer(1024,768);
    TMesh * mesh = new TMesh();
    mesh->pushVertex (VertexData(QVector3D(-1,-1,0),QVector2D(0,0)));
    mesh->pushVertex (VertexData(QVector3D(1,-1,0),QVector2D(1,0)));
    mesh->pushVertex (VertexData(QVector3D(1,1,0),QVector2D(1,1)));
    mesh->pushVertex (VertexData(QVector3D(-1,1,0),QVector2D(0,1)));
    mesh->pushIndex (0);
    mesh->pushIndex (1);
    mesh->pushIndex (2);
    mesh->pushIndex (0);
    mesh->pushIndex (2);
    mesh->pushIndex (3);
    mesh->setMaterial (MaterialPool::getInstance ()->createOrGetMaterial ("default"));
    mesh->finishWithoutNormal ();
    m_quad = new Entity();
    m_quad->addMesh (mesh);

    m_guiCamera = new Camera();
    m_guiCamera->setOrtho (0,1024,0,768,0.01,1000);
    m_guiCamera->setPos (QVector3D(0,0,0));

    m_renderType = DEFERRED_SHADING;
}

void Scene::pushEntityToRenderQueue(Entity *entity)
{
    this->m_entityList.push_back(entity);
}

void Scene::pushSpriteToRenderQueue(Sprite *sprite)
{
    this->m_spriteList.push_back (sprite);
}

void Scene::pushCustomNodeToRenderQueue(Node *node)
{
    m_customNodeList.push_back (node);
}

void Scene::render()
{
    this->m_root.visit (this);
    sortRenderQue();
    switch(m_renderType)
    {
    case FORWARD_SHADING:
    {
        QMessageBox msgBox;
        msgBox.setText("the forward rendering was dropped since ver 0.09b");
        msgBox.exec();
    }
        break;
    case DEFERRED_SHADING:
    {
        for(int i =0;i<m_renderTargetList.size ();i++)
        {
            deferredRendering (m_renderTargetList[i]);
        }
        deferredRendering (m_mainRenderTarget);
    }
        break;
    }
    spriteRenderPass ();
    m_tempEntityList = m_entityList;
    m_entityList.clear ();
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
        entity->getShaderProgram()->setUniformInteger("g_has_animation",1);
        std::vector <Matrix4f > transform;
        entity->bonesTransform(entity->animateTime(),transform,entity->animationName());
        for(int i =0;i<transform.size();i++)
        {
            char str[100];
            sprintf(str,"g_bones[%d]",i);
            entity->getShaderProgram()->setUniformMat4v(str,(const GLfloat*)(transform[i].m),true);
        }
    }else{
        entity->getShaderProgram()->setUniformInteger("g_has_animation",0);
    }
}

void Scene::spriteRenderPass()
{
    PipeLine p;
    for (auto i =m_spriteList.begin();i!=m_spriteList.end();i++)
    {
        Sprite * sprite = (* i);
        sprite->shader ()->use ();
        Camera * camera =sprite->camera ();
        p.setProjectionMatrix(camera->getProjection());
        p.setViewMatrix(camera->getViewMatrix());
        p.setEyePosition (camera->pos ());
        p.setModelMatrix(sprite->getModelTrans());
        p.apply(sprite->shader());

        sprite->draw();
        if(sprite->onRender)
        {
            sprite->onRender(sprite,0);
        }
    }
    m_spriteList.clear ();
}

void Scene::customNodeRenderPass()
{

}

void Scene::shadowPassForSpot(SpotLight * light,RenderTarget * target)
{
    // Clear color and depth buffer
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    light->shadowFBO ()->BindForWriting();
    glClear(GL_DEPTH_BUFFER_BIT);
    ShaderProgram * shadow_shader = ShaderPool::getInstance ()->get("shadow");
    PipeLine p;
    for (std::vector<Entity *>::iterator i =m_entityList.begin();i!=m_entityList.end();i++)
    {
        shadow_shader->use ();
        Entity * entity = (* i);
        if(!entity->isEnableShadow()) continue;
        if(target->isIgnoreEntity (entity))continue;
        Camera * camera =entity->getCamera();
        p.setModelMatrix(target->auxMatrix ()*entity->getModelTrans());
        p.setProjectionMatrix(camera->getProjection());
        QMatrix4x4 view;
        view.setToIdentity();
        auto lightDir = light->getDirection ();
        lightDir.normalize ();
        view.lookAt(light->getPos(),light->getPos()+lightDir,QVector3D(0,1,0));
        p.setViewMatrix(view);
        ShaderProgram * tmp_shader = entity->getShaderProgram ();
        entity->setShaderProgram (shadow_shader);
        setEntityBoneTransform(entity);
        p.apply(shadow_shader);
        entity->draw();
        entity->setShaderProgram (tmp_shader);
    }
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);//switch the frame buffer back.
}

void Scene::shadowPassDirectional(RenderTarget * target)
{
    auto light = this->getDirectionalLight ();
    for(int j =0;j<4;j++)
    {
        // Clear color and depth buffer
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        light->getCSM_FBO ( j )->BindForWriting ();
        glClear(GL_DEPTH_BUFFER_BIT);
        ShaderProgram * shadow_shader = ShaderPool::getInstance ()->get("shadow");
        PipeLine p;
        for (std::vector<Entity *>::iterator i =m_entityList.begin();i!=m_entityList.end();i++)
        {
            shadow_shader->use ();
            Entity * entity = (* i);
            if(!entity->isEnableShadow()) continue;
            if(target->isIgnoreEntity (entity))continue;
            p.setModelMatrix(target->auxMatrix ()*entity->getModelTrans());
            QMatrix4x4 lightView;
            lightView.setToIdentity();
            auto lightDir = directionLight.getDirection ();

            QVector3D pos = QVector3D(0,0,0);
            lightView.lookAt(pos,lightDir,QVector3D(0,1,0));

            auto aabb = camera ()->getSplitFrustumAABB ( j );
            aabb.transForm (camera()->getModelTrans ());
            aabb.transForm (lightView);
            auto matrix = getCropMatrix (aabb);
            p.setProjectionMatrix(matrix);
            p.setViewMatrix(lightView);
            ShaderProgram * tmp_shader = entity->getShaderProgram ();
            entity->setShaderProgram (shadow_shader);
            setEntityBoneTransform(entity);
            p.apply(shadow_shader);
            entity->draw();
            entity->setShaderProgram (tmp_shader);
        }
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);//switch the frame buffer back.
    }
}

void Scene::geometryPass(RenderTarget * target)
{
    target->getGBuffer ()->BindForWriting ();
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_BLEND);
    PipeLine p;
    for (std::vector<Entity *>::iterator i =m_entityList.begin();i!=m_entityList.end();i++)
    {
        Entity * entity = (* i);
        entity->getShaderProgram ()->use ();
        entity->adjustVertices ();
        if(target->isIgnoreEntity (entity))continue;
        if(target->isEnableClipPlane ())
        {
            auto shader = entity->getShaderProgram ();
            auto array = target->getClipPlane ();
            shader->setUniform4Float ("g_clip_plane",array[0],array[1],array[2],array[3]);
            shader->setUniformInteger ("g_is_enable_clip",1);
        }else
        {
            auto shader = entity->getShaderProgram ();
            shader->setUniformInteger ("g_is_enable_clip",0);
        }
        Camera * camera  = target->camera ();
        p.setProjectionMatrix(camera->getProjection());
        p.setViewMatrix(camera->getViewMatrix());
        p.setEyePosition (camera->pos ());
        p.setModelMatrix(target->auxMatrix ()*entity->getModelTrans());
        p.setEyeDirection(camera->getForwardVector ());
        p.applyLightMvp(entity->getShaderProgram());
        setEntityBoneTransform(entity);
        p.apply(entity->getShaderProgram());
        entity->draw();
        if(entity->onRender)
        {
            entity->onRender(entity,0);
        }
    }
    if(m_skyBox && !target->isIgnoreSkyBox ())
    {
        m_skyBox->setShader (ShaderPool::getInstance ()->get ("sky_box_deferred"));
        Camera * camera =target->camera ();
        m_skyBox->shader()->use();
        p.setProjectionMatrix(camera->getProjection());
        p.setViewMatrix(camera->getViewMatrix());
        m_skyBox->getEntity()->translate(camera->pos ().x (),camera->pos ().y (),camera->pos ().z ());
        p.setModelMatrix(target->auxMatrix ()*m_skyBox->getEntity()->getModelTrans());
        p.apply(m_skyBox->shader());
        m_skyBox->Draw();
    }
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
}

void Scene::lightPass(RenderTarget * target)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //pointLightPass(target);
    //spotLightPass(target);
    directionLightPass(target);
}

void Scene::spotLightPass(RenderTarget * target)
{
    if(!this->spotLights.empty ())
    {
        for(int i =0;i<spotLights.size ();i++)
        {
            PointLight * light = this->spotLights[i];
            light->shadowFBO ()->BindForReading(GL_TEXTURE3);
            ShaderProgram * shader = ShaderPool::getInstance ()->get("spot_light_pass");
            shader->use ();
            light->shadowFBO ()->applyShadowMapTexture (shader,3);

            light->apply (shader,0);
            m_quad->setShaderProgram (shader);
            QMatrix4x4 m;
            m.setToIdentity ();
            auto camera = target->camera ();
            shader->setUniformMat4v ("g_MVP_matrix",m.data ());
            shader->setUniform2Float ("g_screen_size",1024,768);
            shader->setUniformInteger ("g_color_map",0);
            shader->setUniformInteger ("g_position_map",1);
            shader->setUniformInteger ("g_normal_map",2);
            shader->setUniform3Float ("g_eye_position",
                                      camera->pos ().x(),
                                      camera->pos ().y(),
                                      camera->pos ().z());
            QMatrix4x4 lightView;
            lightView.setToIdentity();
            lightView.lookAt(spotLights[0]->getPos(),this->spotLights[0]->getPos()+this->spotLights[0]->getDirection(),QVector3D(0,1,0));
            QMatrix4x4 light_vp;
            light_vp = camera->getProjection () * lightView ;
            shader->setUniformMat4v ("g_light_vp_matrix",light_vp.data ());
            m_quad->draw (true);
        }
    }
}

void Scene::pointLightPass(RenderTarget *target)
{
    if(!this->pointLights.empty ())
    {
        for(int i =0;i<pointLights.size ();i++)
        {
            ShaderProgram * shader = ShaderPool::getInstance ()->get("point_light_pass");
            shader->use ();
            PointLight * light = this->pointLights[i];
            light->apply (shader,0);

            m_quad->setShaderProgram (shader);
            QMatrix4x4 m;
            m.setToIdentity ();
            auto camera = target->camera ();
            shader->setUniformMat4v ("g_MVP_matrix",m.data ());
            shader->setUniform2Float ("g_screen_size",1024,768);
            shader->setUniformInteger ("g_color_map",0);
            shader->setUniformInteger ("g_position_map",1);
            shader->setUniformInteger ("g_normal_map",2);
            shader->setUniform3Float ("g_eye_position",
                                      camera->pos ().x(),
                                      camera->pos ().y(),
                                      camera->pos ().z());
            m_quad->draw (true);
        }
    }
}

void Scene::directionLightPass(RenderTarget *target)
{
    ShaderProgram * shader =ShaderPool::getInstance ()->get("dir_light_pass");
    shader->use ();
    int texture_offset = 5;

    for(int i = 0 ;i<4;i++)
    {
        directionLight.getCSM_FBO (i)->BindForReading(GL_TEXTURE0+i+texture_offset);
        char GLSL_shadowMap_name [30];
        sprintf(GLSL_shadowMap_name,"g_shadow_map[%d]",i);
        directionLight.getCSM_FBO (i)->applyShadowMapTexture (shader,i+texture_offset,GLSL_shadowMap_name);
    }

    QMatrix4x4 m;
    m.setToIdentity ();
    m_quad->setShaderProgram (shader);
    shader->setUniformMat4v ("g_MVP_matrix",m.data ());
    shader->setUniform2Float ("g_screen_size",1024,768);
    shader->setUniformInteger ("g_color_map",0);
    shader->setUniformInteger ("g_position_map",1);
    shader->setUniformInteger ("g_normal_map",2);

    shader->setUniformInteger ("g_depth_map",4);//for depth

    auto camera = target->camera ();
    if(camera)
    {
        shader->setUniform3Float ("g_eye_position",
                                  camera->pos ().x(),
                                  camera->pos ().y(),
                                  camera->pos ().z());
    }

    QMatrix4x4 lightView;
    lightView.setToIdentity();


    QVector3D lightDir = directionLight.getDirection ();
    QVector3D pos = QVector3D(0,0,0);
    lightView.lookAt(pos,lightDir,QVector3D(0,1,0));

    for(int i =0 ;i <4 ;i++)
    {
        if(!camera) break;
        auto split_frustum_aabb = camera->getSplitFrustumAABB (i);
        split_frustum_aabb.transForm (target->camera()->getModelTrans ());
        split_frustum_aabb.transForm (lightView);
        auto matrix = getCropMatrix (split_frustum_aabb);
        QMatrix4x4 light_vp;
        light_vp = matrix * lightView ;
        char GLSL_light_VP_name [30];
        sprintf(GLSL_light_VP_name,"g_light_vp_matrix[%d]",i);
        shader->setUniformMat4v (GLSL_light_VP_name,light_vp.data ());
    }
    this->directionLight.apply(shader);
    this->ambientLight.apply(shader);
    m_quad->draw (true);
}

void Scene::calculateLight(ShaderProgram *shader)
{
    shader->setUniformInteger("g_point_light_amount",this->pointLights.size());
    shader->setUniformInteger("g_spot_light_amount",this->spotLights.size());
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
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
void Scene::deferredRendering(RenderTarget * target)
{
    if(target->isEnableClipPlane ())
    {
        glEnable(GL_CLIP_PLANE0);
        glClipPlane(GL_CLIP_PLANE0, target->getClipPlane());
    }
    if(!this->spotLights.empty ())
    {
        this->shadowPassForSpot(spotLights[0],target);
    }
    if(this->directionLight.getIntensity ()>0)
    {
        this->shadowPassDirectional (target);
    }

    geometryPass(target);
    if(target->isEnableClipPlane ())
    {
        glDisable(GL_CLIP_PLANE0);
    }
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
    target->getGBuffer ()->BindForReading(bloom_fbo1->buffer ());

    lightPass(target);

    bloom_fbo1->BindForReading (bloom_fbo2);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    pickBright();
    bloom_fbo2->BindForReading (bloom_fbo3);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gaussianBlur_H (2);
    bloom_fbo3->BindForReading (NULL);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gaussianBlur_V (2);

    if(target->type () == RenderTarget::TargetType::ON_SCREEN)
    {
        bloom_fbo1->BindForReading (NULL);
    }
    else
    {
        bloom_fbo1->BindForReading (target->resultBuffer ());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    ShaderProgram * shader =ShaderPool::getInstance ()->get("deffered_simple");
    shader->use ();
    QMatrix4x4 m;
    m.setToIdentity ();
    m_quad->setShaderProgram (shader);
    auto camera =target->camera ();
    shader->setUniformMat4v ("g_MVP_matrix",m.data ());
    shader->setUniform2Float ("g_screen_size",1024,768);
    shader->setUniformInteger ("g_color_map",0);
    shader->setUniformInteger ("g_position_map",1);
    shader->setUniformInteger ("g_normal_map",2);
    if(camera)
    {
        shader->setUniform3Float ("g_eye_position",
                                  camera->pos ().x(),
                                  camera->pos ().y(),
                                  camera->pos ().z());
    }
    m_quad->draw (true);
}



//use CropMatrix to adjust the projection matrix which Cascaded Shadow Maps need;
QMatrix4x4 Scene::getCropMatrix(AABB frustumAABB)
{
    float scaleX = 2.0f/(frustumAABB.max ().x() - frustumAABB.min ().x());
    float scaleY = 2.0f/(frustumAABB.max ().y() - frustumAABB.min ().y());
    float offsetX = -0.5f*(frustumAABB.max ().x() + frustumAABB.min ().x()) * scaleX;
    float offsetY = -0.5f*(frustumAABB.max ().y() + frustumAABB.min ().y()) * scaleY;

    auto CropMatrix = QMatrix4x4(scaleX,0.0f,  0.0f, offsetX,
                                 0.0f,  scaleY,  0.0f, offsetY,
                                 0.0f,    0.0f,  1.0f,  0.0f,
                                 0.0f, 0.0f,  0.0f,  1.0f );
    auto orthoMatrix = QMatrix4x4();

    orthoMatrix.ortho(-1.0, 1.0, -1.0, 1.0, -frustumAABB.max ().z(), -frustumAABB.min ().z() );
    orthoMatrix = CropMatrix * orthoMatrix;
    return orthoMatrix;
}

Node *Scene::root()
{
    return &m_root;
}
int Scene::renderType() const
{
    return m_renderType;
}

void Scene::setRenderType(int renderType)
{
    m_renderType = renderType;
}

Camera *Scene::camera() const
{
    return m_mainRenderTarget->camera ();
}

void Scene::setCamera(Camera *camera)
{
    m_mainRenderTarget->setCamera (camera);
}
Camera *Scene::guiCamera() const
{
    return m_guiCamera;
}

void Scene::setGuiCamera(Camera *guiCamera)
{
    m_guiCamera = guiCamera;
}

std::vector<Entity *> Scene::getPotentialEntityList() const
{
    return m_tempEntityList;
}

void Scene::addRenderTarget(RenderTarget *target)
{
    m_renderTargetList.push_back (target);
}

bool entityCompare( Entity* pfirst, Entity* psecond)
{
    return pfirst->getDistToCamera ()<psecond->getDistToCamera ();
}

void Scene::sortRenderQue()
{
    std::sort(m_entityList.begin (),m_entityList.end (),entityCompare);
}


void Scene::pickBright()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ShaderProgram * shader =ShaderPool::getInstance ()->get("pick_bright");
    shader->use ();
    QMatrix4x4 m;
    m.setToIdentity ();
    m_quad->setShaderProgram (shader);
    shader->setUniformMat4v ("g_MVP_matrix",m.data ());
    shader->setUniform2Float ("g_screen_size",1024,768);
    shader->setUniformInteger ("g_color_map",0);
    shader->setUniformInteger ("g_position_map",1);
    shader->setUniformInteger ("g_normal_map",2);
    this->directionLight.apply(shader);
    this->ambientLight.apply(shader);
    m_quad->draw (true);
}

void Scene::gaussianBlur_H(float size)
{

    ShaderProgram * shader =ShaderPool::getInstance ()->get("gaussian_blur");
    shader->use ();
    QMatrix4x4 m;
    m.setToIdentity ();
    m_quad->setShaderProgram (shader);
    shader->setUniformFloat ("g_blur_size",size);
    shader->setUniformMat4v ("g_MVP_matrix",m.data ());
    shader->setUniform2Float ("g_screen_size",1024,768);
    shader->setUniformInteger ("g_color_map",0);
    shader->setUniformInteger ("g_position_map",1);
    shader->setUniformInteger ("g_normal_map",2);
    this->directionLight.apply(shader);
    this->ambientLight.apply(shader);
    m_quad->draw (true);
}

void Scene::gaussianBlur_V(float size)
{
    ShaderProgram * shader =ShaderPool::getInstance ()->get("gaussian_blur_v");
    shader->use ();
    QMatrix4x4 m;
    m.setToIdentity ();
    m_quad->setShaderProgram (shader);
    shader->setUniformFloat ("g_blur_size",size);
    shader->setUniformMat4v ("g_MVP_matrix",m.data ());
    shader->setUniform2Float ("g_screen_size",1024,768);
    shader->setUniformInteger ("g_color_map",0);
    shader->setUniformInteger ("g_position_map",1);
    shader->setUniformInteger ("g_normal_map",2);
    this->directionLight.apply(shader);
    this->ambientLight.apply(shader);
    m_quad->draw (true);
}

void Scene::linearBlur(float radius, float samples)
{
    ShaderProgram * shader =ShaderPool::getInstance ()->get("linear_blur");
    shader->use ();
    QMatrix4x4 m;
    m.setToIdentity ();
    m_quad->setShaderProgram (shader);
    shader->setUniformFloat ("g_blur_radius",radius);
    shader->setUniformFloat ("g_blur_samples",samples);
    shader->setUniformMat4v ("g_MVP_matrix",m.data ());
    shader->setUniform2Float ("g_screen_size",1024,768);
    shader->setUniformInteger ("g_color_map",0);
    shader->setUniformInteger ("g_position_map",1);
    shader->setUniformInteger ("g_normal_map",2);
    this->directionLight.apply(shader);
    this->ambientLight.apply(shader);
    m_quad->draw (true);
}
