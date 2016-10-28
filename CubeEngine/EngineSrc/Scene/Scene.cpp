#include "Scene.h"
#include "../Engine/Engine.h"
#include "../Game/ConsolePanel.h"
#include "../Game/DebugInfoPanel.h"
#include "../Interface/Drawable3D.h"
#include "../Engine/EngineDef.h"
namespace tzw {

Scene::Scene()
{
    m_root.setScene(this);
    m_ambient = new AmbientLight();
    m_dirLight = new DirectionalLight();
    m_skyBox = nullptr;
}

Scene *Scene::create()
{
    auto scene = new Scene();
    return scene;
}

void Scene::addNode(Node *node)
{
    m_root.addChild(node);
}

void Scene::visit()
{
    m_root.visit();
}

void Scene::visitPost()
{
    m_root.visitPost(m_octreeScene);
}

void Scene::visitDraw()
{
    if(!defaultCamera()->getUseCustomFrustumUpdate())
    {
        defaultCamera()->updateFrustum();
    }
    m_octreeScene->cullingByCamera(defaultCamera());

    auto &visibleList = m_octreeScene->getVisibleList();
    for(auto obj : visibleList)
    {
        obj->submitDrawCmd();
    }
}

Camera *Scene::defaultGUICamera() const
{
    return m_defaultGUICamera;
}

void Scene::setDefaultGUICamera(Camera *defaultGUICamera)
{
    m_defaultGUICamera = defaultGUICamera;
}
Camera *Scene::defaultCamera() const
{
    return m_defaultCamera;
}

void Scene::setDefaultCamera(Camera *defaultCamera)
{
    m_defaultCamera = defaultCamera;
}

bool Scene::hitByRay(const Ray &ray, vec3 &hitPoint)
{
    return m_octreeScene->hitByRay(ray,hitPoint);
}

void Scene::getRange(std::vector<Drawable3D *> *list, AABB aabb)
{
    m_octreeScene->getRange(list,aabb);
}

Node *Scene::root()
{
    return &m_root;
}

ConsolePanel *Scene::getConsolePanel() const
{
    return m_consolePanel;
}

void Scene::setConsolePanel(ConsolePanel *consolePanel)
{
    m_consolePanel = consolePanel;
}

DebugInfoPanel *Scene::getDebugInfoPanel() const
{
    return m_debugInfoPanel;
}

void Scene::setDebugInfoPanel(DebugInfoPanel *debugInfoPanel)
{
    m_debugInfoPanel = debugInfoPanel;
}

void Scene::createDefaultCameras()
{
    m_defaultCamera = Camera::CreatePerspective(60,1.0f* Engine::shared()->windowWidth()/Engine::shared()->windowHeight(),0.01,200);
    m_defaultGUICamera = Camera::CreateOrtho(0,Engine::shared()->windowWidth(),0,Engine::shared()->windowHeight(),-1,100);
    //m_root.addChild(m_defaultCamera);
}

SkyBox *Scene::getSkyBox() const
{
    return m_skyBox;
}

void Scene::setSkyBox(SkyBox *skyBox)
{
    m_skyBox = skyBox;
}

BaseLight *Scene::getAmbient() const
{
    return m_ambient;
}

OctreeScene *Scene::getOctreeScene() const
{
    return m_octreeScene;
}

void Scene::setOctreeScene(OctreeScene *octreeScene)
{
    m_octreeScene = octreeScene;
}

DirectionalLight *Scene::getDirectionLight()
{
    return m_dirLight;
}

void Scene::init()
{
    createDefaultCameras();
    m_root.setName("root");
    AABB sceneBounding;
    sceneBounding.update(vec3(-2000,-200,-2000));
    sceneBounding.update(vec3(2000,200,2000));
    m_octreeScene = new OctreeScene();
    m_octreeScene->init(sceneBounding);
    m_consolePanel = new ConsolePanel(this->root());
    m_debugInfoPanel = new DebugInfoPanel();
    this->root()->addChild(m_debugInfoPanel);
}

} // namespace tzw

