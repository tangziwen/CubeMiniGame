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
#include "ShadingParams.h"
namespace tzw {
class StdMaterial;
class Node;
class Drawable;

enum MaterialFlag
{
	MaterialFlag_isCullFace = 1 << 1,
    MaterialFlag_isInstanced = 1 << 2,
	MaterialFlag_isBlend = 1 << 3,
	MaterialFlag_isDepthWrite = 1 << 4,
	MaterialFlag_isDepthTest = 1 << 5,
	Shader_option_End = 1 << 31,
};
enum class PrimitiveTopology
{
	TriangleList,
	LineList,
};


class Material : public InspectableUI
{
public:
    Material ();
	void loadFromTemplate(std::string name);
	void loadFromFile(std::string filePath);
	void loadFromJson(rapidjson::Value & obj, std::string envFolder);
    static Material * createFromTemplate(std::string name);
	static Material * createFromFile(std::string matPath);
	static Material * createFromJson(rapidjson::Value & obj, std::string envFolder);
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
    Material * clone();
	void reload();

	bool getIsCullFace();

	void setIsCullFace(bool newVal);
	RenderFlag::CullMode getCullMode();
	void setCullMode(RenderFlag::CullMode newCullMode);
	TechniqueVar * get(std::string);

	void inspect() override;
	void inspectIMGUI(std::string name, float min, float max, const char * fmt = "%.2f");
	void inspectIMGUI_Color(std::string name);
	void handleSemanticValuePassing(TechniqueVar * val, std::string name, ShaderProgram * program);
	bool isIsDepthTestEnable() const;
	void setIsDepthTestEnable(const bool isDepthTestEnable);
	bool isIsDepthWriteEnable() const;
	void setIsDepthWriteEnable(const bool isDepthWriteEnable);
	void setIsEnableInstanced(const bool isEnableInstanced);
	bool isEnableInstanced();
	RenderFlag::BlendingFactor getFactorSrc() const;
	void setFactorSrc(const RenderFlag::BlendingFactor factorSrc);
	RenderFlag::BlendingFactor getFactorDst() const;
	void setFactorDst(const RenderFlag::BlendingFactor factorDst);
	bool isIsEnableBlend() const;
	void setIsEnableBlend(const bool isEnableBlend);
	uint32_t getMutationFlag();
	uint32_t getMaterialFlag();
	void updateFullDescriptionStr();
	const std::string& getFullDescriptionStr();
	std::unordered_map<std::string,TechniqueVar> & getVarList();
	PrimitiveTopology getPrimitiveTopology();
	void setPrimitiveTopology(PrimitiveTopology newTopology);
	ShadingParams * getShadingParams();
private:
    std::string m_vsPath;
    std::string m_fsPath;
	bool m_isEnableInstanced;
	ShadingParams * m_shadingParams;
	RenderFlag::BlendingFactor m_factorSrc;
	RenderFlag::BlendingFactor m_factorDst;
	bool m_isCullFace;
	bool m_isDepthTestEnable;
	bool m_isDepthWriteEnable;
	bool m_isEnableBlend;
	// bool m_isEnableAlphaTest;
	std::string m_name;
	std::unordered_map<std::string, unsigned int> m_texSlotMap;
	ShaderProgram * m_program;
	RenderFlag::RenderStage m_renderStage;
	std::string m_fullDescString;
	RenderFlag::CullMode m_cullMode;
	PrimitiveTopology m_primitiveTopology;
public:
	RenderFlag::RenderStage getRenderStage() const;
	void setRenderStage(const RenderFlag::RenderStage renderStage);
};

} // namespace tzw

#endif // TZW_TECHNIQUE_H
