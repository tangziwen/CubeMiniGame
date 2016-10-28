#ifndef TZW_EFFECT_H
#define TZW_EFFECT_H
#include "EngineSrc/Shader/ShaderProgram.h"
#include <map>
#include "EngineSrc/Technique/TechniqueVar.h"
namespace tzw {
class Drawable;
class Material;
class Effect
{
public:
    Effect();
    void load(std::string name);
    void loadFromFile(std::string filePath);
    std::string name() const;
    unsigned int getMapSlot(std::string mapName);
    unsigned int getMapSlotWithAlias(std::string mapName);
    std::string getAlias(std::string theName);
    ShaderProgram *getProgram() const;
    TechniqueVar getDefaultValue(std::string theName);
    void getAttrList(std::map<std::string, TechniqueVar> & attrMap);
protected:
    std::string m_name;
    std::map<std::string, unsigned int> m_texSlotMap;
    std::map<std::string, std::string> m_aliasMap;
    std::map<std::string, TechniqueVar> m_defaultValue;
    ShaderProgram * m_program;
};
} // namespace tzw

#endif // TZW_EFFECT_H
