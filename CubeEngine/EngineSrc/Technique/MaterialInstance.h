#ifndef TZW_TECHNIQUE_H
#define TZW_TECHNIQUE_H
#include <vector>
#include <string>
#include <map>
#include "../Shader/ShaderProgram.h"
#include "TechniqueVar.h"
#include "../Math/Matrix44.h"
#include "EngineSrc/3D/Effect/Effect.h"
#include "2D/InspectableUI.h"
#include "rapidjson/document.h"
#include "Rendering/RenderFlag.h"
#include "MaterialState.h"
#include "Material.h"
#include "ShadingParams.h"
namespace tzw {
class StdMaterial;
class Node;
class Drawable;


class MaterialInstance : public InspectableUI
{
public:
    MaterialInstance ();
	void loadFromMaterial(std::string name);
	void loadFromMaterial(Material * material);
	void loadFromFile(std::string filePath);
	void loadFromJson(rapidjson::Value & obj, std::string envFolder);
    static MaterialInstance * createFromMaterial(std::string name);
    static MaterialInstance * createFromMaterial(Material * material);
	static MaterialInstance * createFromFile(std::string matPath);
	static MaterialInstance * createFromJson(rapidjson::Value & obj, std::string envFolder);
    void setVar(std::string name, const Matrix44 &value);
    void setVar(std::string name,const float& value);
    void setVar(std::string name,const int& value);
	void setVar(std::string name, const vec2 & value);
    void setVar(std::string name, const vec3& value);
    void setVar(std::string name, const vec4 &value);
    void setVar(std::string name, const TechniqueVar & value);
    void setTex(std::string name,Texture * texture,int id = 0);
    Texture * getTex(std::string name);
    void use(ShaderProgram * extraProgram = nullptr);
	unsigned int getMapSlot(std::string mapName);
	ShaderProgram *getProgram() const;
	bool isExist(std::string name);
    MaterialInstance * clone();
	void reload();

	bool getIsCullFace();
	RenderFlag::CullMode getCullMode();
	TechniqueVar * get(std::string);

	void inspect() override;
	void inspectIMGUI(std::string name, float min, float max, const char * fmt = "%.2f");
	void inspectIMGUI_Color(std::string name);
	void handleSemanticValuePassing(TechniqueVar * val, std::string name, ShaderProgram * program);
	bool isIsDepthTestEnable() const;
	bool isIsDepthWriteEnable() const;
	bool isEnableInstanced();
	RenderFlag::BlendingFactor getFactorSrc() const;
	RenderFlag::BlendingFactor getFactorDst() const;
	bool isIsEnableBlend() const;
	uint32_t getMutationFlag();
	uint32_t getMaterialFlag();
	const std::string& getFullDescriptionStr();
	std::unordered_map<std::string,TechniqueVar> & getVarList();
	PrimitiveTopology getPrimitiveTopology();
	RasterFillMode getRasterFillMode();
	ShadingParams * getShadingParams();
	Material * getMaterial();
	const Material * getMaterial() const;
	Material * ensureUniqueMaterial();
private:
	void applyMaterial(Material * material);
	void applyInstanceOverrides(rapidjson::Value& overrides, std::string envFolder);
	void loadFromInstanceJson(rapidjson::Value& doc, std::string envFolder);
	Material * m_material;
	bool m_isMaterialUnique;
	ShadingParams * m_shadingParams;
	// bool m_isEnableAlphaTest;
	std::string m_name;
public:
	RenderFlag::RenderStage getRenderStage() const;
};

} // namespace tzw

#endif // TZW_TECHNIQUE_H
