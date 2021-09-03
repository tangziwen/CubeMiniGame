#include "ShaderProgram.h"
#include <string.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <stdio.h>
#include "Utility/log/Log.h"
#include "BackEnd/RenderBackEnd.h"
#include "Utility/file/Data.h"
#include "Utility/file/Tfile.h"
#include "ShaderMgr.h"
#include "Engine/Engine.h"

using namespace std;
namespace tzw {

static int BitCount(uint32_t n)
{
    unsigned int c =0 ; // 计数器
    while (n >0)
    {
        if((n &1) ==1) // 当前位是1
            ++c ; // 计数器加1
        n >>=1 ; // 移位
    }
    return c ;
}

ShaderProgram::ShaderProgram(uint32_t mutationFlag, const char *pVSFileName, const char *pFSFileName, const char *pTCSFileName, const char* pTESFileName)
{
    m_mutationFlag = mutationFlag;
	m_fragmentShader = pFSFileName;
	m_vertexShader = pVSFileName;
	m_tessellationControlShader.clear();
	shader = 0;
	if(pTCSFileName)
	{
		m_tessellationControlShader = pTCSFileName;
	}
	m_tessellationEvaluationShader.clear();
	if(pTESFileName)
	{
		m_tessellationEvaluationShader = pTESFileName;
	}
    createShader(true);
	
}

void ShaderProgram::use()
{
    glUseProgram(shader->m_uid);

}

int ShaderProgram::getShaderId()
{
    return this->shader->m_uid;
}

void ShaderProgram::setUniformInteger(const char *str, int value)
{
        int ptr =uniformLocation(str);
        if(ptr!=-1)
        {
            glUniform1i(ptr,value);
			RenderBackEnd::shared()->selfCheck();
        }
        else{
            //T_LOG<<"there is no uniform called:"<<str<<"in shader :"<<this->shader;
        }
}

void ShaderProgram::setUniformMat4v(const char *str, const float *array, bool transpose, int count)
{
        int ptr =uniformLocation(str);
        if(ptr!=-1)
        {
            glUniformMatrix4fv(ptr,count,transpose,array);
			RenderBackEnd::shared()->selfCheck();
        }
        else{
            //T_LOG<<"there is no uniform called:"<<str<<"in shader :"<<this->shader;
        }
}

	void ShaderProgram::setUniform3Floatv(const char* str, const vec3* array, int count)
	{
        int ptr =uniformLocation(str);
        if(ptr!=-1)
        {
			glUniform3fv(ptr, count, (const GLfloat*)&array[0]);  
			RenderBackEnd::shared()->selfCheck();
        }
        else{
            //T_LOG<<"there is no uniform called:"<<str<<"in shader :"<<this->shader;
        }
		
	}

	void ShaderProgram::setUniformFloat(const char *str, float value)
{
        int ptr =uniformLocation(str);
        if(ptr!=-1)
        {
            glUniform1f(ptr,value);
			RenderBackEnd::shared()->selfCheck();
        }
        else{
            //T_LOG<<"there is no uniform called:"<<str<<"in shader :"<<this->shader;
        }
}

void ShaderProgram::setUniform3Float(const char *str, float x, float y, float z)
{
        int ptr =uniformLocation(str);
        if(ptr!=-1)
        {
            glUniform3f(ptr,x,y,z);
			RenderBackEnd::shared()->selfCheck();
        }
        else{
            //printf("there is no uniform called:%s in shader : %d",str,this->shader);
        }
}

void ShaderProgram::setUniform3Float(const char *str, vec3 v)
{
    setUniform3Float(str,v.x,v.y,v.z);
}

void ShaderProgram::setUniform2Float(const char *str, float x, float y)
{
        int ptr =uniformLocation(str);
        if(ptr!=-1)
        {
            glUniform2f(ptr,x,y);
			RenderBackEnd::shared()->selfCheck();
        }
        else{
            //T_LOG<<"there is no uniform called:"<<str<<"in shader :"<<this->shader;
        }
}

void ShaderProgram::setUniform2Float(const char * str, vec2 vec)
{
	setUniform2Float(str, vec.x, vec.y);
}

void ShaderProgram::setUniform4Float(const char *str, float x, float y, float z, float w)
{
        int ptr =uniformLocation(str);
        if(ptr!=-1)
        {
            glUniform4f(ptr,x,y,z,w);
			RenderBackEnd::shared()->selfCheck();
        }
        else{
            //T_LOG<<"there is no uniform called:"<<str<<"in shader :"<<this->shader;
        }
}

void ShaderProgram::setUniform4Float(const char *str, vec4 v)
{
    setUniform4Float(str,v.x,v.y,v.z,v.w);
}

unsigned int ShaderProgram::attributeLocation(string name)
{
    auto result = m_locationMap.find(name);
    if(result!=m_locationMap.end())
    {
        return result->second;
    }else
    {
        auto location = glGetAttribLocation(shader->m_uid,name.c_str());
		RenderBackEnd::shared()->selfCheck();
        m_locationMap.insert(std::make_pair(name,location));
        return location;
    }
}

void ShaderProgram::enableAttributeArray(unsigned int attributeId)
{
    glEnableVertexAttribArray(attributeId);
	RenderBackEnd::shared()->selfCheck();
}

void ShaderProgram::setAttributeBuffer(int ID, int dataType, int offset, int size, int stride)
{
    glVertexAttribPointer(ID,size,dataType,GL_FALSE,stride,reinterpret_cast<void *>(offset));
	RenderBackEnd::shared()->selfCheck();
}

void ShaderProgram::setAttributeBufferInt(int ID, int dataType, int offset, int size, int stride)
{
    glVertexAttribIPointer(ID,size,dataType,stride,reinterpret_cast<void *>(offset));
	RenderBackEnd::shared()->selfCheck();
}

int ShaderProgram::uniformLocation(std::string name)
{
	auto iter = m_uniformMap.find(name);
	if(iter != m_uniformMap.end())
	{
		return iter->second;
	}else
	{
		int ptr =glGetUniformLocation(this->shader->m_uid,name.c_str());
		RenderBackEnd::shared()->selfCheck();
		m_uniformMap[name] = ptr;
		return ptr;
	}
}

void ShaderProgram::reload()
{
	auto oldShader = shader;
	shader = 0;
	m_uniformMap.clear();
	m_locationMap.clear();
	createShader(false);
    delete oldShader;
}

DeviceShaderCollection* ShaderProgram::getDeviceShader()
{
    return shader;
}

void ShaderProgram::processShaderText(const char* pShaderText, std::string & finalStr)
{
	//calculate Global Macro and mutation
	auto mutationSize = BitCount(m_mutationFlag);
	auto globalMacroSize = ShaderMgr::shared()->m_macros.size();
	auto size = globalMacroSize + mutationSize;
    std::string prefix = "";
	auto glsl_ver = "#version 420\n";
    prefix += glsl_ver;
    char * tmpStr = (char *)malloc(128);
	for(const auto &i : ShaderMgr::shared()->m_macros)
	{	
		sprintf(tmpStr, "#define %s %s\n", i.first.c_str(), i.second.c_str());
        prefix+= tmpStr;
	}
	if(m_mutationFlag & static_cast<uint32_t>(ShaderOption::EnableInstanced))
    {
		sprintf(tmpStr, "#define FLAG_EnableInstanced 1\n");
        prefix+= tmpStr;
	}
	if(m_mutationFlag & static_cast<uint32_t>(ShaderOption::EnableDoubleSide))
    {
		sprintf(tmpStr, "#define FLAG_EnableDoubleSide 1\n");
        prefix+= tmpStr;

	}
    prefix += "\n";
	finalStr = prefix + pShaderText;
    free(tmpStr);
}

void ShaderProgram::createShader(bool isStrict)
{
    auto shaderObj = Engine::shared()->getRenderBackEnd()->createShader_imp();
    ;
    this->shader = shaderObj;
    if (!shaderObj->create()) {
        fprintf(stderr, "Error creating shader program\n");
        abort();
    }
    addShader(shaderObj, m_vertexShader, DeviceShaderType::VertexShader, isStrict);
    addShader(shaderObj, m_fragmentShader, DeviceShaderType::FragmentShader, isStrict);

    if(!m_tessellationControlShader.empty())
    {
        addShader(shader, m_tessellationControlShader, DeviceShaderType::TessControlShader, isStrict);
    }

    if(!m_tessellationEvaluationShader.empty())
    {
        addShader(shader, m_tessellationEvaluationShader, DeviceShaderType::TessEvaulateShader, isStrict);
    }
    if(!shaderObj->finish()){
        fprintf(stdout, "Create Shader Failed: \n");
        if(isStrict){
            abort();
        }
    }
}




void ShaderProgram::addShader(DeviceShaderCollection * ShaderProgram, std::string filePath, DeviceShaderType ShaderType, bool isStrict)
{
    tzw::Data data = tzw::Tfile::shared()->getData(filePath,true);
    std::string finalText;
    processShaderText(data.getString().c_str(), finalText);
    ShaderProgram->addShader((const unsigned char *)finalText.c_str(), finalText.size(),ShaderType, (const unsigned char *)filePath.c_str());
}

} // namespace tzw

