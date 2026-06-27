#include "Material.h"
#include "../Shader/ShaderMgr.h"
#include "Engine/Engine.h"
#include "Texture/TextureMgr.h"
#include "Utility/file/Tfile.h"
#include "Utility/log/Log.h"
#include <cstring>
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
    if(str == "front" || str == "Front")
    {
        return RenderFlag::CullMode::Front;
    }
    if(str == "back" || str == "Back")
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
    if(str == "Fill")
    {
        return RasterFillMode::Fill;
    }
    return defaultMode;
}

void applyRenderState(MaterialTechnique& technique, rapidjson::Value& doc)
{
    if(doc.HasMember("cullFace"))
    {
        technique.m_isCullFace = doc["cullFace"].GetBool();
    }
    if(doc.HasMember("CullMode"))
    {
        technique.m_cullMode = parseCullMode(doc["CullMode"].GetString(), technique.m_cullMode);
    }
    if(doc.HasMember("RasterFillMode"))
    {
        technique.m_rasterFillMode = parseRasterFillMode(doc["RasterFillMode"].GetString(), technique.m_rasterFillMode);
    }
    if(doc.HasMember("DepthTestEnable"))
    {
        technique.m_isDepthTestEnable = doc["DepthTestEnable"].GetBool();
    }
    if(doc.HasMember("DepthWriteEnable"))
    {
        technique.m_isDepthWriteEnable = doc["DepthWriteEnable"].GetBool();
    }
    if(doc.HasMember("BlendEnable"))
    {
        technique.m_isEnableBlend = doc["BlendEnable"].GetBool();
    }
    if(doc.HasMember("EnableInstanced"))
    {
        technique.m_isEnableInstanced = doc["EnableInstanced"].GetBool();
    }
    if(doc.HasMember("SrcBlendFactor"))
    {
        technique.m_factorSrc = parseBlendFactor(doc["SrcBlendFactor"].GetString(), technique.m_factorSrc);
    }
    if(doc.HasMember("DstBlendFactor"))
    {
        technique.m_factorDst = parseBlendFactor(doc["DstBlendFactor"].GetString(), technique.m_factorDst);
    }
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

void loadTextureMapSlot(std::unordered_map<std::string, unsigned int>& texSlotMap, rapidjson::Value& tex)
{
    std::string name = tex[0].GetString();
    texSlotMap[name] = tex[1].GetInt();
}

void loadDefaultTextureMapParam(ShadingParams& params, rapidjson::Value& tex, std::string envFolder)
{
    std::string name = tex[0].GetString();
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

void loadDefaultTextureMap(ShadingParams& params, std::unordered_map<std::string, unsigned int>& texSlotMap, rapidjson::Value& tex, std::string envFolder)
{
    loadTextureMapSlot(texSlotMap, tex);
    loadDefaultTextureMapParam(params, tex, envFolder);
}

std::unordered_map<std::string, Material*>& getMaterialCache()
{
    static std::unordered_map<std::string, Material*> cache;
    return cache;
}

}

Material::Material()
    : m_renderStage(RenderFlag::RenderStage::COMMON)
{
    m_techniques[MaterialTechniqueType::Default] = MaterialTechnique();
}

Material * Material::clone() const
{
    auto material = new Material();
    material->m_sourcePath = m_sourcePath;
    material->m_name = m_name;
    material->m_shadingParams.copyFrom(m_shadingParams);
    material->m_texSlotMap = m_texSlotMap;
    material->m_techniques = m_techniques;
    material->m_renderStage = m_renderStage;
    return material;
}

void Material::loadFromFile(std::string filePath)
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

void Material::loadFromJson(rapidjson::Value& doc, std::string envFolder)
{
    m_shadingParams = ShadingParams();
    m_texSlotMap.clear();
    m_techniques.clear();
    m_techniques[MaterialTechniqueType::Default] = MaterialTechnique();
    m_renderStage = RenderFlag::RenderStage::COMMON;

    if(doc.HasMember("name"))
    {
        m_name = doc["name"].GetString();
    }

    if(doc.HasMember("RenderStage"))
    {
        m_renderStage = parseRenderStage(doc["RenderStage"].GetString(), m_renderStage);
    }

    if(doc.HasMember("shaders"))
    {
        tlog("[error] legacy top-level shaders are not supported anymore. file=%s material=%s missing techniques.default", m_sourcePath.c_str(), m_name.c_str());
        abort();
    }

    if(!doc.HasMember("techniques") || !doc["techniques"].IsObject() || !doc["techniques"].HasMember("default"))
    {
        tlog("[error] material missing techniques.default. file=%s material=%s", m_sourcePath.c_str(), m_name.c_str());
        abort();
    }

    MaterialTechnique defaultRenderState;
    applyRenderState(defaultRenderState, doc);

    auto& techniques = doc["techniques"];
    for(auto iter = techniques.MemberBegin(); iter != techniques.MemberEnd(); ++iter)
    {
        if(!iter->value.IsObject())
        {
            tlog("[error] material technique must be an object. file=%s material=%s technique=%s", m_sourcePath.c_str(), m_name.c_str(), iter->name.GetString());
            abort();
        }
        auto type = materialTechniqueTypeFromString(iter->name.GetString());
        auto& technique = m_techniques[type];
        loadTechnique(technique, iter->value, envFolder, defaultRenderState);
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
            loadDefaultTextureMap(m_shadingParams, m_texSlotMap, texMap[i], envFolder);
        }
    }

    updateFullDescriptionStr();
}

Material * Material::getFromLib(std::string name)
{
    return getFromFile(std::string("MatLibs/") + name + ".mat");
}

Material * Material::getFromFile(std::string filePath)
{
    auto& cache = getMaterialCache();
    auto iter = cache.find(filePath);
    if(iter != cache.end())
    {
        return iter->second;
    }

    auto material = new Material();
    material->loadFromFile(filePath);
    cache[filePath] = material;
    return material;
}

ShaderProgram * Material::getProgram() const
{
    return getDefaultTechnique().m_program;
}

unsigned int Material::getMapSlot(std::string mapName)
{
    return m_texSlotMap[mapName];
}

bool Material::getIsCullFace() const
{
    return getDefaultTechnique().m_isCullFace;
}

void Material::setIsCullFace(bool newVal)
{
    bool needReload = false;
    for(auto& item : m_techniques)
    {
        auto& technique = item.second;
        if(technique.m_isCullFace != newVal)
        {
            technique.m_isCullFace = newVal;
            needReload = true;
        }
    }
    if(needReload)
    {
        reload();
    }
}

void Material::setIsCullFace(MaterialTechniqueType type, bool newVal)
{
    auto& technique = getTechnique(type);
    if(technique.m_isCullFace == newVal)
    {
        return;
    }
    technique.m_isCullFace = newVal;
    reload();
}

RenderFlag::CullMode Material::getCullMode() const
{
    return getDefaultTechnique().m_cullMode;
}

void Material::setCullMode(RenderFlag::CullMode newCullMode)
{
    for(auto& item : m_techniques)
    {
        item.second.m_cullMode = newCullMode;
    }
    updateFullDescriptionStr();
}

void Material::setCullMode(MaterialTechniqueType type, RenderFlag::CullMode newCullMode)
{
    getTechnique(type).m_cullMode = newCullMode;
    updateFullDescriptionStr();
}

bool Material::isIsDepthTestEnable() const
{
    return getDefaultTechnique().m_isDepthTestEnable;
}

void Material::setIsDepthTestEnable(bool isDepthTestEnable)
{
    for(auto& item : m_techniques)
    {
        item.second.m_isDepthTestEnable = isDepthTestEnable;
    }
    updateFullDescriptionStr();
}

void Material::setIsDepthTestEnable(MaterialTechniqueType type, bool isDepthTestEnable)
{
    getTechnique(type).m_isDepthTestEnable = isDepthTestEnable;
    updateFullDescriptionStr();
}

bool Material::isIsDepthWriteEnable() const
{
    return getDefaultTechnique().m_isDepthWriteEnable;
}

void Material::setIsDepthWriteEnable(bool isDepthWriteEnable)
{
    for(auto& item : m_techniques)
    {
        item.second.m_isDepthWriteEnable = isDepthWriteEnable;
    }
    updateFullDescriptionStr();
}

void Material::setIsDepthWriteEnable(MaterialTechniqueType type, bool isDepthWriteEnable)
{
    getTechnique(type).m_isDepthWriteEnable = isDepthWriteEnable;
    updateFullDescriptionStr();
}

bool Material::isEnableInstanced() const
{
    return getDefaultTechnique().m_isEnableInstanced;
}

void Material::setIsEnableInstanced(bool isEnableInstanced)
{
    bool needReload = false;
    for(auto& item : m_techniques)
    {
        auto& technique = item.second;
        if(technique.m_isEnableInstanced != isEnableInstanced)
        {
            technique.m_isEnableInstanced = isEnableInstanced;
            needReload = true;
        }
    }
    if(needReload)
    {
        reload();
    }
}

void Material::setIsEnableInstanced(MaterialTechniqueType type, bool isEnableInstanced)
{
    auto& technique = getTechnique(type);
    if(technique.m_isEnableInstanced == isEnableInstanced)
    {
        return;
    }
    technique.m_isEnableInstanced = isEnableInstanced;
    reload();
}

RenderFlag::BlendingFactor Material::getFactorSrc() const
{
    return getDefaultTechnique().m_factorSrc;
}

void Material::setFactorSrc(RenderFlag::BlendingFactor factorSrc)
{
    for(auto& item : m_techniques)
    {
        item.second.m_factorSrc = factorSrc;
    }
    updateFullDescriptionStr();
}

void Material::setFactorSrc(MaterialTechniqueType type, RenderFlag::BlendingFactor factorSrc)
{
    getTechnique(type).m_factorSrc = factorSrc;
    updateFullDescriptionStr();
}

RenderFlag::BlendingFactor Material::getFactorDst() const
{
    return getDefaultTechnique().m_factorDst;
}

void Material::setFactorDst(RenderFlag::BlendingFactor factorDst)
{
    for(auto& item : m_techniques)
    {
        item.second.m_factorDst = factorDst;
    }
    updateFullDescriptionStr();
}

void Material::setFactorDst(MaterialTechniqueType type, RenderFlag::BlendingFactor factorDst)
{
    getTechnique(type).m_factorDst = factorDst;
    updateFullDescriptionStr();
}

bool Material::isIsEnableBlend() const
{
    return getDefaultTechnique().m_isEnableBlend;
}

void Material::setIsEnableBlend(bool isEnableBlend)
{
    for(auto& item : m_techniques)
    {
        item.second.m_isEnableBlend = isEnableBlend;
    }
    updateFullDescriptionStr();
}

void Material::setIsEnableBlend(MaterialTechniqueType type, bool isEnableBlend)
{
    getTechnique(type).m_isEnableBlend = isEnableBlend;
    updateFullDescriptionStr();
}

PrimitiveTopology Material::getPrimitiveTopology() const
{
    return getDefaultTechnique().m_primitiveTopology;
}

void Material::setPrimitiveTopology(PrimitiveTopology newTopology)
{
    for(auto& item : m_techniques)
    {
        item.second.m_primitiveTopology = newTopology;
    }
    updateFullDescriptionStr();
}

void Material::setPrimitiveTopology(MaterialTechniqueType type, PrimitiveTopology newTopology)
{
    getTechnique(type).m_primitiveTopology = newTopology;
    updateFullDescriptionStr();
}

RasterFillMode Material::getRasterFillMode() const
{
    return getDefaultTechnique().m_rasterFillMode;
}

void Material::setRasterFillMode(RasterFillMode newMode)
{
    for(auto& item : m_techniques)
    {
        item.second.m_rasterFillMode = newMode;
    }
    updateFullDescriptionStr();
}

void Material::setRasterFillMode(MaterialTechniqueType type, RasterFillMode newMode)
{
    getTechnique(type).m_rasterFillMode = newMode;
    updateFullDescriptionStr();
}

RenderFlag::RenderStage Material::getRenderStage() const
{
    return m_renderStage;
}

void Material::setRenderStage(RenderFlag::RenderStage renderStage)
{
    m_renderStage = renderStage;
    updateFullDescriptionStr();
}

void Material::setDefaultVar(std::string name, const float& value)
{
    m_shadingParams.setVar(name, value);
}

void Material::setDefaultVar(std::string name, const int& value)
{
    m_shadingParams.setVar(name, value);
}

void Material::setDefaultVar(std::string name, const vec2& value)
{
    m_shadingParams.setVar(name, value);
}

void Material::setDefaultVar(std::string name, const vec3& value)
{
    m_shadingParams.setVar(name, value);
}

void Material::setDefaultVar(std::string name, const vec4& value)
{
    m_shadingParams.setVar(name, value);
}

void Material::setDefaultTex(std::string name, Texture* texture)
{
    m_shadingParams.setTex(name, texture);
}

uint32_t Material::getMutationFlag() const
{
    return getDefaultTechnique().getMutationFlag();
}

uint32_t Material::getMaterialFlag() const
{
    return getDefaultTechnique().getMaterialFlag();
}

const std::string& Material::getFullDescriptionStr() const
{
    return getDefaultTechnique().getFullDescriptionStr();
}

void Material::reload()
{
    for(auto& item : m_techniques)
    {
        auto& technique = item.second;
        if(!technique.m_vsPath.empty() && !technique.m_fsPath.empty())
        {
            technique.m_program = ShaderMgr::shared()->getByPath(technique.getMutationFlag(), technique.m_vsPath, technique.m_fsPath);
            if(!technique.m_program)
            {
                tlog("[error] bad program!!! file=%s material=%s technique=%s", m_sourcePath.c_str(), m_name.c_str(), materialTechniqueTypeToString(item.first));
                abort();
            }
        }
    }
    updateFullDescriptionStr();
}

MaterialTechnique& Material::getTechnique(MaterialTechniqueType type)
{
    auto iter = m_techniques.find(type);
    if(iter == m_techniques.end())
    {
        tlog("[error] material missing technique. file=%s material=%s technique=%s", m_sourcePath.c_str(), m_name.c_str(), materialTechniqueTypeToString(type));
        abort();
    }
    return iter->second;
}

const MaterialTechnique& Material::getTechnique(MaterialTechniqueType type) const
{
    auto iter = m_techniques.find(type);
    if(iter == m_techniques.end())
    {
        tlog("[error] material missing technique. file=%s material=%s technique=%s", m_sourcePath.c_str(), m_name.c_str(), materialTechniqueTypeToString(type));
        abort();
    }
    return iter->second;
}

std::vector<MaterialTechniqueType> Material::getTechniqueTypes() const
{
    std::vector<MaterialTechniqueType> result;
    result.reserve(m_techniques.size());
    for(auto& item : m_techniques)
    {
        result.emplace_back(item.first);
    }
    return result;
}

MaterialTechnique& Material::getDefaultTechnique()
{
    return getTechnique(MaterialTechniqueType::Default);
}

const MaterialTechnique& Material::getDefaultTechnique() const
{
    return getTechnique(MaterialTechniqueType::Default);
}

void Material::loadTechnique(MaterialTechnique& technique, rapidjson::Value& doc, std::string envFolder, const MaterialTechnique& defaultRenderState)
{
    (void)envFolder;
    technique = defaultRenderState;
    applyRenderState(technique, doc);

    if(!doc.HasMember("shaders"))
    {
        tlog("[error] material technique missing shaders. file=%s material=%s technique=default", m_sourcePath.c_str(), m_name.c_str());
        abort();
    }

    auto& shaders = doc["shaders"];
    technique.m_vsPath = shaders["vs"].GetString();
    technique.m_fsPath = shaders["fs"].GetString();
    if(Engine::shared()->getRenderDeviceType() == RenderDeviceType::Vulkan_Device)
    {
        technique.m_vsPath = "Vulkan" + technique.m_vsPath;
        technique.m_fsPath = "Vulkan" + technique.m_fsPath;
    }
    technique.m_program = ShaderMgr::shared()->getByPath(technique.getMutationFlag(), technique.m_vsPath, technique.m_fsPath);
    if(!technique.m_program)
    {
        tlog("[error] bad program!!! file=%s material=%s technique=default", m_sourcePath.c_str(), m_name.c_str());
        abort();
    }
    updateTechniqueFullDescriptionStr(technique);
}

void Material::updateTechniqueFullDescriptionStr(MaterialTechnique& technique)
{
    technique.updateFullDescriptionStr(m_renderStage);
}

void Material::updateFullDescriptionStr()
{
    for(auto& item : m_techniques)
    {
        updateTechniqueFullDescriptionStr(item.second);
    }
}

} // namespace tzw
