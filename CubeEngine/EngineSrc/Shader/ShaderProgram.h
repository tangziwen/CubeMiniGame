#ifndef TZW_SHADERPROGRAM_H
#define TZW_SHADERPROGRAM_H
#include "../Math/vec3.h"
#include "../Math/vec4.h"
#include "../Math/vec2.h"
#include <string>
#include <map>
#include <unordered_map>
#include "BackEnd/DeviceShaderCollection.h"
namespace tzw {
enum class ShaderOption
{
	EnableInstanced = 1 << 1,
    EnableDoubleSide = 1 << 2,
	Shader_option_End = 1 << 31,
};
class DeviceShaderCollection;
class ShaderProgram
{
public:
    ShaderProgram(uint32_t mutationFlag, const char * pVSFileName, const char * pFSFileName, const char * pTCSFileName = nullptr, const char* pTESFileName = nullptr);
    void use();
    int getShaderId();
    void setUniformInteger(const char * str,int value);
    void setUniformMat4v(const char * str, const float *array, bool transpose = false, int count = 1);
	void setUniform3Floatv(const char * str, const vec3 * array, int count);
    void setUniformFloat(const char * str,float value);
    void setUniform3Float(const char * str,float x,float y,float z);
    void setUniform3Float(const char * str,vec3 v);
    void setUniform2Float(const char * str,float x,float y);
	void setUniform2Float(const char * str, vec2 vec);
    void setUniform4Float(const char * str,float x,float y,float z,float w);
    void setUniform4Float(const char * str,vec4 v);
    unsigned int attributeLocation(std::string name);
    void enableAttributeArray(unsigned int attributeId);
    void setAttributeBuffer(int ID, int dataType, int offset, int size, int stride = 0);
	void setAttributeBufferInt(int ID, int dataType, int offset, int size, int stride = 0);
	int uniformLocation(std::string name);
	void reload();
    DeviceShaderCollection * getDeviceShader();
	std::string m_fragmentShader;
	std::string m_vertexShader;
	std::string m_tessellationControlShader;
	std::string m_tessellationEvaluationShader;
private:
	uint32_t m_mutationFlag;
    void processShaderText(const char* pShaderText, std::string & finalStr);
	void createShader(bool isStrict);
    void addShader(DeviceShaderCollection * ShaderProgram, std::string filePath, DeviceShaderType ShaderType, bool isStrict);
    std::unordered_map<std::string,unsigned int> m_locationMap;
	std::unordered_map<std::string, int> m_uniformMap;
    DeviceShaderCollection* shader;
};
} // namespace tzw

#endif // TZW_SHADERPROGRAM_H
