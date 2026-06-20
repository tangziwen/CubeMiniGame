#ifndef TZW_MATERIALPOOL_H
#define TZW_MATERIALPOOL_H
#include "EngineSrc/Engine/EngineDef.h"
#include "MaterialInstance.h"
#include <map>
#include "Mesh/Mesh.h"

namespace tzw {

class MaterialPool : public Singleton<MaterialPool>
{
public:
    MaterialPool();
    MaterialInstance * getMaterialByName(std::string name);
    void addMaterial(std::string materialName, MaterialInstance * mat);
    Mesh * getMeshByName(std::string name);
    void addMesh(std::string materialName, Mesh * mat);
    std::string getModelMangleedName(std::string modelName);
	MaterialInstance * getMatFromTemplate(std::string effectName);
	MaterialInstance * getMatFromTemplateWithUniqueName(std::string effectName, std::string uniqueName);
private:
    std::map<std::string, MaterialInstance *> m_materialMap;
	std::map<std::string, Mesh *> m_modelMap;
};

} // namespace tzw

#endif // TZW_MATERIALPOOL_H
