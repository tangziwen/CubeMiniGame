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

Mesh * MaterialPool::getMeshByName(std::string name)
{
    auto result = m_modelMap.find(name);
    if(result != m_modelMap.end())
    {
        return result->second;
    }
    return nullptr;
}

void MaterialPool::addMesh(std::string name, Mesh* mesh)
{
	m_modelMap[name] = mesh;
}

std::string MaterialPool::getModelMangleedName(std::string modelName)
{
    return std::string("model_") + modelName;
}

Material * MaterialPool::getMatFromTemplate(std::string effectName)
{
	auto mat = getMaterialByName(effectName);
	if (!mat)
	{
		mat = Material::createFromTemplate(effectName);
		addMaterial(effectName, mat);
	}
	return mat;
}

} // namespace tzw
