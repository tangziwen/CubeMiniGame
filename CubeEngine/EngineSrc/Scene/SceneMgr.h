#ifndef TZW_SCENEMGR_H
#define TZW_SCENEMGR_H

#include "Scene.h"
#include "../Engine/EngineDef.h"
namespace tzw {

class SceneMgr
{
public:
    TZW_SINGLETON_DECL(SceneMgr)
    Scene *currentScene() const;
    void setCurrentScene(Scene *currentScene);
    void autoSwitch();
    void doVisit();
    void doVisitPost();
    void doVisitDraw();
    void init();
private:
    SceneMgr();
    Scene * m_currentScene;
    Scene * m_candidate;
};

} // namespace tzw

#endif // TZW_SCENEMGR_H
