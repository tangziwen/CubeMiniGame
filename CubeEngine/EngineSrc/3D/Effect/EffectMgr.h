#ifndef TZW_EFFECTMGR_H
#define TZW_EFFECTMGR_H

#include "EngineSrc/Engine/EngineDef.h"
#include "Effect.h"
#include <map>
namespace tzw {

class EffectMgr : public Singleton<EffectMgr>
{
public:
    EffectMgr();
    Effect * get(std::string name);
    void initBuiltIn();
    void addEffect(std::string name, Effect * theEffect);
    void addEffect(std::string effectName);
private:
    std::map<std::string, Effect *> m_effectMap;

};
} // namespace tzw

#endif // TZW_EFFECTMGR_H
