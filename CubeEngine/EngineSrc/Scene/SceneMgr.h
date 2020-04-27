#ifndef TZW_SCENEMGR_H
#define TZW_SCENEMGR_H

#include "Scene.h"
#include "../Engine/EngineDef.h"
namespace tzw {

Scene * g_GetCurrScene();
class SceneMgr : public Singleton<SceneMgr>
{
public:
    Scene *getCurrScene() const;
    void setCurrentScene(Scene *getCurrScene);
    void autoSwitch();
    void doVisit();
    void init();
	SceneMgr();
private:
    
    Scene * m_currentScene;
    Scene * m_candidate;
};

} // namespace tzw

#endif // TZW_SCENEMGR_H
