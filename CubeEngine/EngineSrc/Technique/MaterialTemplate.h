#ifndef TZW_MATERIAL_TEMPLATE_H
#define TZW_MATERIAL_TEMPLATE_H

#include <string>
#include <unordered_map>
#include <cstdint>
#include "MaterialState.h"
#include "ShadingParams.h"
#include "rapidjson/document.h"

namespace tzw
{

class Material;

class MaterialTemplate
{
public:
    MaterialTemplate();
    MaterialTemplate(const MaterialTemplate& other) = default;
    MaterialTemplate * clone() const;
    void loadFromFile(std::string filePath);
    void loadFromJson(rapidjson::Value& doc, std::string envFolder);

    static MaterialTemplate * getFromTemplate(std::string name);
    static MaterialTemplate * getFromFile(std::string filePath);

    ShaderProgram * getProgram() const;
    unsigned int getMapSlot(std::string mapName);
    bool getIsCullFace() const;
    void setIsCullFace(bool newVal);
    RenderFlag::CullMode getCullMode() const;
    void setCullMode(RenderFlag::CullMode newCullMode);
    bool isIsDepthTestEnable() const;
    void setIsDepthTestEnable(bool isDepthTestEnable);
    bool isIsDepthWriteEnable() const;
    void setIsDepthWriteEnable(bool isDepthWriteEnable);
    bool isEnableInstanced() const;
    void setIsEnableInstanced(bool isEnableInstanced);
    RenderFlag::BlendingFactor getFactorSrc() const;
    void setFactorSrc(RenderFlag::BlendingFactor factorSrc);
    RenderFlag::BlendingFactor getFactorDst() const;
    void setFactorDst(RenderFlag::BlendingFactor factorDst);
    bool isIsEnableBlend() const;
    void setIsEnableBlend(bool isEnableBlend);
    PrimitiveTopology getPrimitiveTopology() const;
    void setPrimitiveTopology(PrimitiveTopology newTopology);
    RasterFillMode getRasterFillMode() const;
    void setRasterFillMode(RasterFillMode newMode);
    RenderFlag::RenderStage getRenderStage() const;
    void setRenderStage(RenderFlag::RenderStage renderStage);
    uint32_t getMutationFlag() const;
    uint32_t getMaterialFlag() const;
    const std::string& getFullDescriptionStr() const;
    void reload();

private:
    friend class MaterialInstance;

    void updateFullDescriptionStr();

    std::string m_sourcePath;
    std::string m_name;
    std::string m_vsPath;
    std::string m_fsPath;
    bool m_isEnableInstanced;
    ShadingParams m_shadingParams;
    RenderFlag::BlendingFactor m_factorSrc;
    RenderFlag::BlendingFactor m_factorDst;
    bool m_isCullFace;
    bool m_isDepthTestEnable;
    bool m_isDepthWriteEnable;
    bool m_isEnableBlend;
    std::unordered_map<std::string, unsigned int> m_texSlotMap;
    ShaderProgram * m_program;
    RenderFlag::RenderStage m_renderStage;
    RenderFlag::CullMode m_cullMode;
    PrimitiveTopology m_primitiveTopology;
    RasterFillMode m_rasterFillMode;
    std::string m_fullDescString;
};

} // namespace tzw

#endif // TZW_MATERIAL_TEMPLATE_H
