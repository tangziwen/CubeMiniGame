#ifndef TZW_MATERIAL_TECHNIQUE_H
#define TZW_MATERIAL_TECHNIQUE_H

#include <cstdint>
#include <string>
#include "MaterialState.h"
#include "Rendering/RenderFlag.h"

namespace tzw
{

class ShaderProgram;

enum class MaterialTechniqueType
{
    Default,
};

const char* materialTechniqueTypeToString(MaterialTechniqueType type);
MaterialTechniqueType materialTechniqueTypeFromString(const std::string& name);

struct MaterialTechnique
{
    MaterialTechnique();

    uint32_t getMutationFlag() const;
    uint32_t getMaterialFlag() const;
    const std::string& getFullDescriptionStr() const;
    void updateFullDescriptionStr(RenderFlag::RenderStage renderStage);

    std::string m_vsPath;
    std::string m_fsPath;
    bool m_isEnableInstanced;
    RenderFlag::BlendingFactor m_factorSrc;
    RenderFlag::BlendingFactor m_factorDst;
    bool m_isCullFace;
    bool m_isDepthTestEnable;
    bool m_isDepthWriteEnable;
    bool m_isEnableBlend;
    ShaderProgram* m_program;
    RenderFlag::CullMode m_cullMode;
    PrimitiveTopology m_primitiveTopology;
    RasterFillMode m_rasterFillMode;
    std::string m_fullDescString;
};

} // namespace tzw

#endif // TZW_MATERIAL_TECHNIQUE_H
