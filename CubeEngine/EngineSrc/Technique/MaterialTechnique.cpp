#include "MaterialTechnique.h"
#include "Shader/ShaderProgram.h"
#include "Utility/log/Log.h"
#include <cstdlib>
#include <sstream>

namespace tzw
{

const char* materialTechniqueTypeToString(MaterialTechniqueType type)
{
    switch(type)
    {
    case MaterialTechniqueType::Default:
        return "default";
    }
    return "default";
}

MaterialTechniqueType materialTechniqueTypeFromString(const std::string& name)
{
    if(name == "default")
    {
        return MaterialTechniqueType::Default;
    }
    tlog("[error] unknown material technique type: %s", name.c_str());
    abort();
    return MaterialTechniqueType::Default;
}

MaterialTechnique::MaterialTechnique()
    : m_isEnableInstanced(false)
    , m_factorSrc(RenderFlag::BlendingFactor::SrcAlpha)
    , m_factorDst(RenderFlag::BlendingFactor::OneMinusSrcAlpha)
    , m_isCullFace(true)
    , m_isDepthTestEnable(true)
    , m_isDepthWriteEnable(true)
    , m_isEnableBlend(false)
    , m_program(nullptr)
    , m_cullMode(RenderFlag::CullMode::Back)
    , m_primitiveTopology(PrimitiveTopology::TriangleList)
    , m_rasterFillMode(RasterFillMode::Fill)
{
}

uint32_t MaterialTechnique::getMutationFlag() const
{
    uint32_t flag = 0;
    if(m_isEnableInstanced)
    {
        flag |= static_cast<uint32_t>(ShaderOption::EnableInstanced);
    }

    if(!m_isCullFace)
    {
        flag |= static_cast<uint32_t>(ShaderOption::EnableDoubleSide);
    }
    return flag;
}

uint32_t MaterialTechnique::getMaterialFlag() const
{
    uint32_t flag = 0;
    if(m_isCullFace)
    {
        flag |= MaterialFlag_isCullFace;
    }
    if(m_isEnableInstanced)
    {
        flag |= MaterialFlag_isInstanced;
    }
    if(m_isEnableBlend)
    {
        flag |= MaterialFlag_isBlend;
    }
    if(m_isDepthWriteEnable)
    {
        flag |= MaterialFlag_isDepthWrite;
    }
    if(m_isDepthTestEnable)
    {
        flag |= MaterialFlag_isDepthTest;
    }
    return flag;
}

const std::string& MaterialTechnique::getFullDescriptionStr() const
{
    return m_fullDescString;
}

void MaterialTechnique::updateFullDescriptionStr(RenderFlag::RenderStage renderStage)
{
    std::ostringstream ostr;
    if(m_program)
    {
        ostr << "shader:" << m_program->m_vertexShader << "|" << m_program->m_fragmentShader;
    }
    else
    {
        ostr << "shader:" << m_vsPath << "|" << m_fsPath;
    }
    ostr << "|technique:" << materialTechniqueTypeToString(MaterialTechniqueType::Default)
        << "|mutation:" << getMutationFlag()
        << "|stage:" << static_cast<int>(renderStage)
        << "|flags:" << static_cast<uint32_t>(getMaterialFlag())
        << "|topology:" << static_cast<int>(m_primitiveTopology)
        << "|raster:" << static_cast<int>(m_rasterFillMode)
        << "|cull:" << static_cast<int>(m_cullMode)
        << "|srcBlend:" << static_cast<int>(m_factorSrc)
        << "|dstBlend:" << static_cast<int>(m_factorDst);
    m_fullDescString = ostr.str();
}

} // namespace tzw
