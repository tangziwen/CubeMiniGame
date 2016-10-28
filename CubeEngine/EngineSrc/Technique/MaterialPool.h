#ifndef TZW_MATERIALPOOL_H
#define TZW_MATERIALPOOL_H
#include "EngineSrc/Engine/EngineDef.h"
#include "Material.h"
#include <map>
namespace tzw {

class MaterialPool
{
public:
    MaterialPool();
    Material * getMaterialByName(std::string name);
    void addMaterial(std::string materialName, Material * mat);
    std::string getModelMangleedName(std::string modelName);
private:
    std::map<std::string, Material *> m_materialMap;
    TZW_SINGLETON_DECL(MaterialPool);
};

} // namespace tzw

#endif // TZW_MATERIALPOOL_H
