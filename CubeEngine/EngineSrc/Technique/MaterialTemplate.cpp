#include "MaterialTemplate.h"
#include "../Shader/ShaderMgr.h"
#include "Engine/Engine.h"
#include "Texture/TextureMgr.h"
#include "Utility/file/Tfile.h"
#include "Utility/log/Log.h"
#include <cstring>
#include <sstream>
#include <unordered_map>

namespace tzw
{
namespace
{

RenderFlag::BlendingFactor parseBlendFactor(const std::string& str, RenderFlag::BlendingFactor defaultFactor)
{
    if(str == "One")
    {
        return RenderFlag::BlendingFactor::One;
    }
    if(str == "Zero")
    {
        return RenderFlag::BlendingFactor::Zero;
    }
    if(str == "SrcAlpha")
    {
        return RenderFlag::BlendingFactor::SrcAlpha;
    }
    if(str == "OneMinusSrcAlpha")
    {
        return RenderFlag::BlendingFactor::OneMinusSrcAlpha;
    }
    if(str == "ConstantAlpha")
    {
        return RenderFlag::BlendingFactor::ConstantAlpha;
    }
    return defaultFactor;
}

RenderFlag::RenderStage parseRenderStage(const std::string& str, RenderFlag::RenderStage defaultStage)
{
    if(str == "COMMON")
    {
        return RenderFlag::RenderStage::COMMON;
    }
    if(str == "TRANSPARENT")
    {
        return RenderFlag::RenderStage::TRANSPARENT;
    }
    if(str == "AFTER_DEPTH_CLEAR")
    {
        return RenderFlag::RenderStage::AFTER_DEPTH_CLEAR;
    }
    return defaultStage;
}

RenderFlag::CullMode parseCullMode(const std::string& str, RenderFlag::CullMode defaultMode)
{
    if(str == "front")
    {
        return RenderFlag::CullMode::Front;
    }
    if(str == "back")
    {
        return RenderFlag::CullMode::Back;
    }
    return defaultMode;
}

RasterFillMode parseRasterFillMode(const std::string& str, RasterFillMode defaultMode)
{
    if(str == "Wireframe")
    {
        return RasterFillMode::Wireframe;
    }
    return defaultMode;
}

void loadAttribute(ShadingParams& params, rapidjson::Value& attribute)
{
    auto theName = attribute["name"].GetString();
    bool hasDefaultVal = true;
    rapidjson::GenericValue<rapidjson::UTF8<>> val;
    if(attribute.HasMember("default"))
    {
        val = attribute["default"];
        if(val.IsArray() && val.Size() <= 0)
        {
            hasDefaultVal = false;
        }
    }
    else
    {
        hasDefaultVal = false;
    }

    std::string typeStr = attribute["type"].GetString();
    TechniqueVar var;
    if(typeStr == "int")
    {
        if(hasDefaultVal)
        {
            var.setI(val.GetInt());
        }
        if(attribute.HasMember("ui_info"))
        {
            auto& uiInfo = attribute["ui_info"];
            if(uiInfo.HasMember("range"))
            {
                var.data.i_min = uiInfo["range"][0].GetInt();
                var.data.i_max = uiInfo["range"][1].GetInt();
            }
        }
    }
    else if(typeStr == "float")
    {
        if(hasDefaultVal)
        {
            var.setF(val.GetDouble());
        }
        if(attribute.HasMember("ui_info"))
        {
            auto& uiInfo = attribute["ui_info"];
            if(uiInfo.HasMember("range"))
            {
                var.data.f_min = uiInfo["range"][0].GetDouble();
                var.data.f_max = uiInfo["range"][1].GetDouble();
            }
        }
    }
    else if(typeStr == "vec2")
    {
        if(hasDefaultVal)
        {
            var.setV2(vec2(val[0].GetDouble(), val[1].GetDouble()));
        }
    }
    else if(typeStr == "vec3")
    {
        if(hasDefaultVal)
        {
            var.setV3(vec3(val[0].GetDouble(), val[1].GetDouble(), val[2].GetDouble()));
        }
    }
    else if(typeStr == "vec4")
    {
        if(hasDefaultVal)
        {
            var.setV4(vec4(val[0].GetDouble(), val[1].GetDouble(), val[2].GetDouble(), val[3].GetDouble()));
        }
    }
    else if(typeStr.find("semantic_") == 0)
    {
        if(typeStr == "semantic_WinSize")
        {
            var.setAsSemantic(TechniqueVar::SemanticType::WIN_SIZE);
        }
        else if(typeStr == "semantic_Model")
        {
            var.setAsSemantic(TechniqueVar::SemanticType::Model);
        }
        else if(typeStr == "semantic_ViewProjectInverted")
        {
            var.setAsSemantic(TechniqueVar::SemanticType::InvertedViewProj);
        }
        else if(typeStr == "semantic_CameraPos")
        {
            var.setAsSemantic(TechniqueVar::SemanticType::CamPos);
        }
        else if(typeStr == "semantic_CameraDir")
        {
            var.setAsSemantic(TechniqueVar::SemanticType::CamDir);
        }
        else if(typeStr == "semantic_SunDirection")
        {
            var.setAsSemantic(TechniqueVar::SemanticType::SunDirection);
        }
        else if(typeStr == "semantic_SunColor")
        {
            var.setAsSemantic(TechniqueVar::SemanticType::SunColor);
        }
        else if(typeStr == "semantic_CamInfo")
        {
            var.setAsSemantic(TechniqueVar::SemanticType::CamInfo);
        }
    }
    params.setVar(theName, var);
}

std::string resolvePathInFolder(const std::string& filePath, const std::string& envFolder)
{
    auto filePathInFolder = Tfile::shared()->toAbsFilePath(filePath, envFolder);
    if(Tfile::shared()->isExist(filePathInFolder))
    {
        return filePathInFolder;
    }
    return filePath;
}

void loadTextureMap(ShadingParams& params, std::unordered_map<std::string, unsigned int>& texSlotMap, rapidjson::Value& tex, std::string envFolder)
{
    std::string name = tex[0].GetString();
    texSlotMap[name] = tex[1].GetInt();

    if(tex.Size() > 2 && strlen(tex[2].GetString()))
    {
        auto texturePath = resolvePathInFolder(tex[2].GetString(), envFolder);
        Texture * texture = TextureMgr::shared()->getByPath(texturePath, true);
        params.setTex(name, texture);
    }
    else
    {
        if(name == "DiffuseMap")
        {
            params.setTex(name, TextureMgr::shared()->getByPath("Texture/BuiltInTexture/defaultMetallic.png", true));
        }
        else if(name == "MetallicMap")
        {
            params.setTex(name, TextureMgr::shared()->getByPath("Texture/BuiltInTexture/defaultMetallic.png", true));
        }
        else if(name == "RoughnessMap")
        {
            params.setTex(name, TextureMgr::shared()->getByPath("Texture/BuiltInTexture/defaultRoughnessMap.png", true));
        }
        else if(name == "NormalMap")
        {
            params.setTex(name, TextureMgr::shared()->getByPath("Texture/BuiltInTexture/defaultNormalMap.png", true));
        }
        else
        {
            params.setTex(name, nullptr);
        }
    }
}

std::unordered_map<std::string, MaterialTemplate*>& getTemplateCache()
{
    static std::unordered_map<std::string, MaterialTemplate*> cache;
    return cache;
}

}

MaterialTemplate::MaterialTemplate()
    : m_isEnableInstanced(false)
    , m_factorSrc(RenderFlag::BlendingFactor::SrcAlpha)
    , m_factorDst(RenderFlag::BlendingFactor::OneMinusSrcAlpha)
    , m_isCullFace(true)
    , m_isDepthTestEnable(true)
    , m_isDepthWriteEnable(true)
    , m_isEnableBlend(false)
    , m_program(nullptr)
    , m_renderStage(RenderFlag::RenderStage::COMMON)
    , m_cullMode(RenderFlag::CullMode::Back)
    , m_primitiveTopology(PrimitiveTopology::TriangleList)
    , m_rasterFillMode(RasterFillMode::Fill)
{
}

void MaterialTemplate::loadFromFile(std::string filePath)
{
    rapidjson::Document doc;
    auto data = Tfile::shared()->getData(filePath, true);
    doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
    if(doc.HasParseError())
    {
        tlog("[error] get json data err! %s %d offset %d", filePath.c_str(), doc.GetParseError(), doc.GetErrorOffset());
        abort();
    }

    m_sourcePath = filePath;
    loadFromJson(doc, Tfile::shared()->getFolder(filePath));
}

void MaterialTemplate::loadFromJson(rapidjson::Value& doc, std::string envFolder)
{
    m_shadingParams = ShadingParams();
    m_texSlotMap.clear();

    if(doc.HasMember("name"))
    {
        m_name = doc["name"].GetString();
    }

    m_isCullFace = doc.HasMember("cullFace") ? doc["cullFace"].GetBool() : true;
    m_cullMode = doc.HasMember("CullMode") ? parseCullMode(doc["CullMode"].GetString(), RenderFlag::CullMode::Back) : RenderFlag::CullMode::Back;
    m_rasterFillMode = doc.HasMember("RasterFillMode") ? parseRasterFillMode(doc["RasterFillMode"].GetString(), RasterFillMode::Fill) : RasterFillMode::Fill;
    m_isDepthTestEnable = doc.HasMember("DepthTestEnable") ? doc["DepthTestEnable"].GetBool() : true;
    m_isDepthWriteEnable = doc.HasMember("DepthWriteEnable") ? doc["DepthWriteEnable"].GetBool() : true;
    m_isEnableBlend = doc.HasMember("BlendEnable") ? doc["BlendEnable"].GetBool() : false;
    m_isEnableInstanced = doc.HasMember("EnableInstanced") ? doc["EnableInstanced"].GetBool() : false;
    m_factorSrc = doc.HasMember("SrcBlendFactor") ? parseBlendFactor(doc["SrcBlendFactor"].GetString(), RenderFlag::BlendingFactor::SrcAlpha) : RenderFlag::BlendingFactor::SrcAlpha;
    m_factorDst = doc.HasMember("DstBlendFactor") ? parseBlendFactor(doc["DstBlendFactor"].GetString(), RenderFlag::BlendingFactor::OneMinusSrcAlpha) : RenderFlag::BlendingFactor::OneMinusSrcAlpha;
    if(doc.HasMember("RenderStage"))
    {
        m_renderStage = parseRenderStage(doc["RenderStage"].GetString(), m_renderStage);
    }

    if(doc.HasMember("shaders"))
    {
        auto& shaders = doc["shaders"];
        m_vsPath = shaders["vs"].GetString();
        m_fsPath = shaders["fs"].GetString();
        if(Engine::shared()->getRenderDeviceType() == RenderDeviceType::Vulkan_Device)
        {
            m_vsPath = "Vulkan" + m_vsPath;
            m_fsPath = "Vulkan" + m_fsPath;
        }
        uint32_t mutationFlag = 0;
        if(m_isEnableInstanced)
        {
            mutationFlag |= static_cast<uint32_t>(ShaderOption::EnableInstanced);
        }
        if(!m_isCullFace)
        {
            mutationFlag |= static_cast<uint32_t>(ShaderOption::EnableDoubleSide);
        }
        m_program = ShaderMgr::shared()->getByPath(mutationFlag, m_vsPath, m_fsPath);
        if(!m_program)
        {
            tlog("[error] bad program!!!");
            abort();
        }
    }

    if(!doc.HasMember("property"))
    {
        updateFullDescriptionStr();
        return;
    }

    auto& materialInfo = doc["property"];
    if(materialInfo.HasMember("attributes"))
    {
        auto& attributes = materialInfo["attributes"];
        for(unsigned int i = 0; i < attributes.Size(); i++)
        {
            loadAttribute(m_shadingParams, attributes[i]);
        }
    }

    if(materialInfo.HasMember("maps"))
    {
        auto& texMap = materialInfo["maps"];
        for(unsigned int i = 0; i < texMap.Size(); i++)
        {
            loadTextureMap(m_shadingParams, m_texSlotMap, texMap[i], envFolder);
        }
    }

    updateFullDescriptionStr();
}

MaterialTemplate * MaterialTemplate::getFromTemplate(std::string name)
{
    return getFromFile(std::string("MatTemplate/") + name + ".mat");
}

MaterialTemplate * MaterialTemplate::getFromFile(std::string filePath)
{
    auto& cache = getTemplateCache();
    auto iter = cache.find(filePath);
    if(iter != cache.end())
    {
        return iter->second;
    }

    auto materialTemplate = new MaterialTemplate();
    materialTemplate->loadFromFile(filePath);
    cache[filePath] = materialTemplate;
    return materialTemplate;
}

ShaderProgram * MaterialTemplate::getProgram() const
{
    return m_program;
}

unsigned int MaterialTemplate::getMapSlot(std::string mapName)
{
    return m_texSlotMap[mapName];
}

bool MaterialTemplate::getIsCullFace() const
{
    return m_isCullFace;
}

void MaterialTemplate::setIsCullFace(bool newVal)
{
    if(m_isCullFace == newVal)
    {
        return;
    }
    m_isCullFace = newVal;
    reload();
}

RenderFlag::CullMode MaterialTemplate::getCullMode() const
{
    return m_cullMode;
}

void MaterialTemplate::setCullMode(RenderFlag::CullMode newCullMode)
{
    m_cullMode = newCullMode;
    updateFullDescriptionStr();
}

bool MaterialTemplate::isIsDepthTestEnable() const
{
    return m_isDepthTestEnable;
}

void MaterialTemplate::setIsDepthTestEnable(bool isDepthTestEnable)
{
    m_isDepthTestEnable = isDepthTestEnable;
    updateFullDescriptionStr();
}

bool MaterialTemplate::isIsDepthWriteEnable() const
{
    return m_isDepthWriteEnable;
}

void MaterialTemplate::setIsDepthWriteEnable(bool isDepthWriteEnable)
{
    m_isDepthWriteEnable = isDepthWriteEnable;
    updateFullDescriptionStr();
}

bool MaterialTemplate::isEnableInstanced() const
{
    return m_isEnableInstanced;
}

void MaterialTemplate::setIsEnableInstanced(bool isEnableInstanced)
{
    if(m_isEnableInstanced == isEnableInstanced)
    {
        return;
    }
    m_isEnableInstanced = isEnableInstanced;
    reload();
}

RenderFlag::BlendingFactor MaterialTemplate::getFactorSrc() const
{
    return m_factorSrc;
}

void MaterialTemplate::setFactorSrc(RenderFlag::BlendingFactor factorSrc)
{
    m_factorSrc = factorSrc;
    updateFullDescriptionStr();
}

RenderFlag::BlendingFactor MaterialTemplate::getFactorDst() const
{
    return m_factorDst;
}

void MaterialTemplate::setFactorDst(RenderFlag::BlendingFactor factorDst)
{
    m_factorDst = factorDst;
    updateFullDescriptionStr();
}

bool MaterialTemplate::isIsEnableBlend() const
{
    return m_isEnableBlend;
}

void MaterialTemplate::setIsEnableBlend(bool isEnableBlend)
{
    m_isEnableBlend = isEnableBlend;
    updateFullDescriptionStr();
}

PrimitiveTopology MaterialTemplate::getPrimitiveTopology() const
{
    return m_primitiveTopology;
}

void MaterialTemplate::setPrimitiveTopology(PrimitiveTopology newTopology)
{
    m_primitiveTopology = newTopology;
    updateFullDescriptionStr();
}

RasterFillMode MaterialTemplate::getRasterFillMode() const
{
    return m_rasterFillMode;
}

void MaterialTemplate::setRasterFillMode(RasterFillMode newMode)
{
    m_rasterFillMode = newMode;
    updateFullDescriptionStr();
}

RenderFlag::RenderStage MaterialTemplate::getRenderStage() const
{
    return m_renderStage;
}

void MaterialTemplate::setRenderStage(RenderFlag::RenderStage renderStage)
{
    m_renderStage = renderStage;
    updateFullDescriptionStr();
}

uint32_t MaterialTemplate::getMutationFlag() const
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

uint32_t MaterialTemplate::getMaterialFlag() const
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

const std::string& MaterialTemplate::getFullDescriptionStr() const
{
    return m_fullDescString;
}

void MaterialTemplate::reload()
{
    if(!m_vsPath.empty() && !m_fsPath.empty())
    {
        m_program = ShaderMgr::shared()->getByPath(getMutationFlag(), m_vsPath, m_fsPath);
        if(!m_program)
        {
            tlog("[error] bad program!!!");
            abort();
        }
    }
    updateFullDescriptionStr();
}

void MaterialTemplate::updateFullDescriptionStr()
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
    ostr << "|mutation:" << getMutationFlag()
        << "|stage:" << static_cast<int>(m_renderStage)
        << "|flags:" << static_cast<uint32_t>(getMaterialFlag())
        << "|topology:" << static_cast<int>(m_primitiveTopology)
        << "|raster:" << static_cast<int>(m_rasterFillMode)
        << "|cull:" << static_cast<int>(m_cullMode)
        << "|srcBlend:" << static_cast<int>(m_factorSrc)
        << "|dstBlend:" << static_cast<int>(m_factorDst);
    m_fullDescString = ostr.str();
}

} // namespace tzw
