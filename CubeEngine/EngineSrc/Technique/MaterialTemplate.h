#ifndef TZW_MATERIAL_TEMPLATE_H
#define TZW_MATERIAL_TEMPLATE_H

#include <string>
#include <unordered_map>
#include "Material.h"

namespace tzw
{

class MaterialTemplate
{
public:
    MaterialTemplate();
    void loadFromFile(std::string filePath);
    void loadFromJson(rapidjson::Value& doc, std::string envFolder);

    static MaterialTemplate * getFromTemplate(std::string name);
    static MaterialTemplate * getFromFile(std::string filePath);

private:
    friend class Material;

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
};

} // namespace tzw

#endif // TZW_MATERIAL_TEMPLATE_H
