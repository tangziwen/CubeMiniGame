#include "SceneMgr.h"
namespace tzw {

TZW_SINGLETON_IMPL(SceneMgr)

SceneMgr::SceneMgr()
    :m_candidate(nullptr)
{
    m_currentScene = nullptr;
}

Scene *SceneMgr::currentScene() const
{
    return m_currentScene;
}

void SceneMgr::setCurrentScene(Scene *currentScene)
{
    if(!m_currentScene)
    {
        m_currentScene = currentScene;
    }else
    {
        m_candidate = currentScene;
    }
}

void SceneMgr::autoSwitch()
{
    if(m_candidate)
    {
        m_currentScene = m_candidate;
        m_candidate = nullptr;
    }
}

void SceneMgr::doVisit()
{
    autoSwitch();
    if(m_currentScene)
    {
        m_currentScene->visit();
    }
}

void SceneMgr::doVisitPost()
{
    if(m_currentScene)
    {
        m_currentScene->visitPost();
    }
}

void SceneMgr::doVisitDraw()
{
    if(m_currentScene)
    {
        m_currentScene->visitDraw();
    }
}


void SceneMgr::init()
{
    m_currentScene = new Scene();
    m_currentScene->init();
}
} // namespace tzw

