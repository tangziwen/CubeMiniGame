#ifndef TZW_MATERIAL_H
#define TZW_MATERIAL_H

#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include "MaterialTechnique.h"
#include "ShadingParams.h"
#include "rapidjson/document.h"

namespace tzw
{

class Texture;

class Material
{
public:
    Material();
    Material(const Material& other) = default;
    Material * clone() const;
    void loadFromFile(std::string filePath);
    void loadFromJson(rapidjson::Value& doc, std::string envFolder);

    static Material * getFromLib(std::string name);
    static Material * getFromFile(std::string filePath);

    MaterialTechnique& getTechnique(MaterialTechniqueType type);
    const MaterialTechnique& getTechnique(MaterialTechniqueType type) const;
    std::vector<MaterialTechniqueType> getTechniqueTypes() const;
    MaterialTechnique& getDefaultTechnique();
    const MaterialTechnique& getDefaultTechnique() const;
    ShaderProgram * getProgram() const;
    unsigned int getMapSlot(std::string mapName);
    bool getIsCullFace() const;
    void setIsCullFace(bool newVal);
    void setIsCullFace(MaterialTechniqueType type, bool newVal);
    RenderFlag::CullMode getCullMode() const;
    void setCullMode(RenderFlag::CullMode newCullMode);
    void setCullMode(MaterialTechniqueType type, RenderFlag::CullMode newCullMode);
    bool isIsDepthTestEnable() const;
    void setIsDepthTestEnable(bool isDepthTestEnable);
    void setIsDepthTestEnable(MaterialTechniqueType type, bool isDepthTestEnable);
    bool isIsDepthWriteEnable() const;
    void setIsDepthWriteEnable(bool isDepthWriteEnable);
    void setIsDepthWriteEnable(MaterialTechniqueType type, bool isDepthWriteEnable);
    bool isEnableInstanced() const;
    void setIsEnableInstanced(bool isEnableInstanced);
    void setIsEnableInstanced(MaterialTechniqueType type, bool isEnableInstanced);
    RenderFlag::BlendingFactor getFactorSrc() const;
    void setFactorSrc(RenderFlag::BlendingFactor factorSrc);
    void setFactorSrc(MaterialTechniqueType type, RenderFlag::BlendingFactor factorSrc);
    RenderFlag::BlendingFactor getFactorDst() const;
    void setFactorDst(RenderFlag::BlendingFactor factorDst);
    void setFactorDst(MaterialTechniqueType type, RenderFlag::BlendingFactor factorDst);
    bool isIsEnableBlend() const;
    void setIsEnableBlend(bool isEnableBlend);
    void setIsEnableBlend(MaterialTechniqueType type, bool isEnableBlend);
    PrimitiveTopology getPrimitiveTopology() const;
    void setPrimitiveTopology(PrimitiveTopology newTopology);
    void setPrimitiveTopology(MaterialTechniqueType type, PrimitiveTopology newTopology);
    RasterFillMode getRasterFillMode() const;
    void setRasterFillMode(RasterFillMode newMode);
    void setRasterFillMode(MaterialTechniqueType type, RasterFillMode newMode);
    RenderFlag::RenderStage getRenderStage() const;
    void setRenderStage(RenderFlag::RenderStage renderStage);
    void setDefaultVar(std::string name, const float& value);
    void setDefaultVar(std::string name, const int& value);
    void setDefaultVar(std::string name, const vec2& value);
    void setDefaultVar(std::string name, const vec3& value);
    void setDefaultVar(std::string name, const vec4& value);
    void setDefaultTex(std::string name, Texture* texture);
    uint32_t getMutationFlag() const;
    uint32_t getMaterialFlag() const;
    const std::string& getFullDescriptionStr() const;
    void reload();

private:
    friend class MaterialInstance;

    void loadTechnique(MaterialTechnique& technique, rapidjson::Value& doc, std::string envFolder, const MaterialTechnique& defaultRenderState);
    void updateTechniqueFullDescriptionStr(MaterialTechnique& technique);
    void updateFullDescriptionStr();

    std::string m_sourcePath;
    std::string m_name;
    ShadingParams m_shadingParams;
    std::unordered_map<std::string, unsigned int> m_texSlotMap;
    std::unordered_map<MaterialTechniqueType, MaterialTechnique> m_techniques;
    RenderFlag::RenderStage m_renderStage;
};

} // namespace tzw

#endif // TZW_MATERIAL_H
