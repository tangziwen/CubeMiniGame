#include "EffectMgr.h"
#include "Base/Log.h"
#include <iostream>
namespace tzw {

TZW_SINGLETON_IMPL(EffectMgr);
EffectMgr::EffectMgr()
{
    initBuiltIn();
	tlog("init built in effect finish\n");
}

Effect *EffectMgr::get(std::string name)
{
    auto result = m_effectMap.find(name);
    if(result == m_effectMap.end())
    {
        std::cout << "no such effect named: " << name << std::endl;
        return nullptr;
    }else
    {
        return m_effectMap[name];
    }
}

void EffectMgr::initBuiltIn()
{
    addEffect("Color");
    addEffect("DirectLight");
    addEffect("ModelColor");
    addEffect("ModelStd");
    addEffect("Simple");
    addEffect("SkyBox");
    addEffect("Sprite");
    addEffect("SpriteColor");
    addEffect("VoxelTerrain");
	addEffect("Grass");
	addEffect("Sky");
	addEffect("GUIColor");
}

void EffectMgr::addEffect(std::string name, Effect *theEffect)
{
    m_effectMap[name] = theEffect;
}

void EffectMgr::addEffect(std::string effectName)
{
    auto effect = new Effect();
    effect->load(effectName);
    addEffect(effectName, effect);
}

} // namespace tzw
