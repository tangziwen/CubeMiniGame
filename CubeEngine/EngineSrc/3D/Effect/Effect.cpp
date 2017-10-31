#include "Effect.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include "External/TUtility/TUtility.h"
#include <iostream>
#include "EngineSrc/Texture/TextureMgr.h"
#include "EngineSrc/Interface/Drawable.h"
#include "EngineSrc/Shader/ShaderMgr.h"
namespace tzw {

Effect::Effect()
{
}

void Effect::load(std::string name)
{
    loadFromFile(std::string("./Res/Effects/") + name + ".effect");
}

void Effect::loadFromFile(std::string filePath)
{
    rapidjson::Document doc;
    auto data = Tfile::shared()->getData(filePath,true);
    doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
    if (doc.HasParseError())
    {
        printf("get json data err! %d offset %d\n",doc.GetParseError(), doc.GetErrorOffset());
        return;
    }
    if(doc.HasMember("name"))
    {
        m_name = doc["name"].GetString();
    }

	if (doc.HasMember("cullFace"))
	{
		m_isCullFace = doc["cullFace"].GetBool();
	}
	else
	{
		m_isCullFace = true;
	}

    if(doc.HasMember("shaders"))
    {
        auto& shaders = doc["shaders"];
        auto vsFilePath = shaders["vs"].GetString();
        auto fsFilePath = shaders["fs"].GetString();
        m_program = ShaderMgr::shared()->getByPath(vsFilePath, fsFilePath);
    }
    auto& MaterialInfo = doc["property"];
    if(MaterialInfo.HasMember("attributes"))
    {
        auto& attributes = MaterialInfo["attributes"];
        for(unsigned int i = 0; i < attributes.Size(); i++)
        {
            auto& attribute = attributes[i];

            auto theName = attribute[0].GetString();
            auto aliasName = attribute[1].GetString();
            auto& val = attribute[3];
            std::string typeStr = attribute[2].GetString();
            m_aliasMap[theName] = aliasName;
            if( typeStr == "int")
            {
                TechniqueVar var;
                var.setI(val.GetInt());
                m_defaultValue[theName] = var;
            }
            else if(typeStr == "float")
            {
                TechniqueVar var;
                var.setF(val.GetDouble());
                m_defaultValue[theName] = var;
            }
            else if(typeStr == "vec3")
            {
                TechniqueVar var;
                var.setV3(vec3(val[0].GetDouble(), val[1].GetDouble(), val[2].GetDouble()));
                m_defaultValue[theName] = var;
            }
            else if(typeStr == "vec4")
            {
                TechniqueVar var;
                var.setV4(vec4(val[0].GetDouble(), val[1].GetDouble(), val[2].GetDouble(), val[3].GetDouble()));
                m_defaultValue[theName] = var;
            }
        }
    }

    if(MaterialInfo.HasMember("maps"))
    {
        auto& texMap = MaterialInfo["maps"];
        for(unsigned int i = 0; i < texMap.Size(); i++)
        {
            auto& tex = texMap[i];
            auto name = tex[0].GetString();
            auto aliasName = tex[1].GetString();
            m_aliasMap[name] = aliasName;
            m_texSlotMap[m_aliasMap[name]] = tex[2].GetInt();
        }
    }
}

std::string Effect::name() const
{
    return m_name;
}

unsigned int Effect::getMapSlot(std::string mapName)
{
    return m_texSlotMap[mapName];
}

unsigned int Effect::getMapSlotWithAlias(std::string mapName)
{
    return getMapSlot(getAlias(mapName));
}

std::string Effect::getAlias(std::string theName)
{
    auto result = m_aliasMap.find(theName);
    if(result == m_aliasMap.end())
    {
        return theName;
    }else
    {
        return m_aliasMap[theName];
    }

}

ShaderProgram *Effect::getProgram() const
{
    return m_program;
}

TechniqueVar Effect::getDefaultValue(std::string theName)
{
    auto result = m_defaultValue.find(theName);
    if (result != m_defaultValue.end())
    {
        return m_defaultValue[theName];
    }else
    {
        TechniqueVar var;
        return var;
    }
}

void Effect::getAttrList(std::map<std::string, TechniqueVar> &attrMap)
{
    attrMap = m_defaultValue;
}

bool Effect::getIsCullFace()
{
	return m_isCullFace;
}

void Effect::setIsCullFace(bool newVal)
{
	m_isCullFace = newVal;
}

} // namespace tzw
