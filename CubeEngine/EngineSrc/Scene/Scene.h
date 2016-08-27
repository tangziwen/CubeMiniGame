#ifndef TZW_SCENE_H
#define TZW_SCENE_H

#include "../Base/Node.h"
#include "../Base/Camera.h"

#include "../Math/Ray.h"
#include "OctreeScene.h"
#include "../Lighting/DirectionalLight.h"
#include "../Lighting/AmbientLight.h"
namespace tzw {

class ConsolePanel;
class DebugInfoPanel;

class Scene
{
public:
    Scene();
    static Scene * create();
    void init();
    void addNode(Node * node);
    void visit();
    void visitPost();
    void visitDraw();
    Camera *defaultGUICamera() const;
    void setDefaultGUICamera(Camera *defaultGUICamera);

    Camera *defaultCamera() const;
    void setDefaultCamera(Camera *defaultCamera);

    bool hitByRay(const Ray &ray, vec3 &hitPoint);
    void getRange(std::vector<Drawable3D *>* list,AABB aabb);
    Node * root();
    ConsolePanel *getConsolePanel() const;
    void setConsolePanel(ConsolePanel *consolePanel);

    DebugInfoPanel *getDebugInfoPanel() const;
    void setDebugInfoPanel(DebugInfoPanel *debugInfoPanel);

    OctreeScene *getOctreeScene() const;
    void setOctreeScene(OctreeScene *octreeScene);

    DirectionalLight *getDirectionLight();

    BaseLight *getAmbient() const;

private:
    DirectionalLight *m_dirLight;
    ConsolePanel * m_consolePanel;
    DebugInfoPanel * m_debugInfoPanel;
    Camera * m_defaultGUICamera;
    Camera * m_defaultCamera;
    void createDefaultCameras();
    AmbientLight * m_ambient;
    Node m_root;
    OctreeScene * m_octreeScene;
};

} // namespace tzw

#endif // TZW_SCENE_H
