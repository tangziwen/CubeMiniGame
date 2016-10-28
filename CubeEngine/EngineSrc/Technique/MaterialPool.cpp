#include "MaterialPool.h"

namespace tzw {
TZW_SINGLETON_IMPL(MaterialPool);

MaterialPool::MaterialPool()
{

}

Material *MaterialPool::getMaterialByName(std::string name)
{
    auto result = m_materialMap.find(name);
    if(result != m_materialMap.end())
    {
        return result->second;
    }
    return nullptr;
}

void MaterialPool::addMaterial(std::string materialName, Material *mat)
{
    m_materialMap[materialName] = mat;
}

std::string MaterialPool::getModelMangleedName(std::string modelName)
{
    return std::string("model_") + modelName;
}


} // namespace tzw
