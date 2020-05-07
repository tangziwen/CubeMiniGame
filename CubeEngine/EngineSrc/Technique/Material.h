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
#include "Rendering/RenderFlag.h"
namespace tzw {
class StdMaterial;
class Node;
class Drawable;
class Material : public InspectableUI
{
public:
    Material ();
	void loadFromTemplate(std::string name);
	void loadFromFile(std::string filePath);
    static Material * createFromTemplate(std::string name);
	static Material * createFromFile(std::string matPath);
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

	bool getIsCullFace();

	void setIsCullFace(bool newVal);

	TechniqueVar * get(std::string);

	void inspect() override;
	void inspectIMGUI(std::string name, float min, float max, const char * fmt = "%.2f");
	void inspectIMGUI_Color(std::string name);
	void handleSemanticValuePassing(TechniqueVar * val, std::string name, ShaderProgram * program);
	bool isIsDepthTestEnable() const;
	void setIsDepthTestEnable(const bool isDepthTestEnable);
	bool isIsDepthWriteEnable() const;
	void setIsDepthWriteEnable(const bool isDepthWriteEnable);
	RenderFlag::BlendingFactor getFactorSrc() const;
	void setFactorSrc(const RenderFlag::BlendingFactor factorSrc);
	RenderFlag::BlendingFactor getFactorDst() const;
	void setFactorDst(const RenderFlag::BlendingFactor factorDst);
	bool isIsEnableBlend() const;
	void setIsEnableBlend(const bool isEnableBlend);
private:
    std::string m_vsPath;
    std::string m_fsPath;
    std::map<std::string,TechniqueVar *> m_varList;
	RenderFlag::BlendingFactor m_factorSrc;
	RenderFlag::BlendingFactor m_factorDst;
	bool m_isCullFace;
	bool m_isDepthTestEnable;
	bool m_isDepthWriteEnable;
	bool m_isEnableBlend;
	// bool m_isEnableAlphaTest;
	std::string m_name;
	std::map<std::string, unsigned int> m_texSlotMap;
	ShaderProgram * m_program;
	RenderFlag::RenderStage m_renderStage;
public:
	RenderFlag::RenderStage getRenderStage() const;
	void setRenderStage(const RenderFlag::RenderStage renderStage);
};

} // namespace tzw

#endif // TZW_TECHNIQUE_H
