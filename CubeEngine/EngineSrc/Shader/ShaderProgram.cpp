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
using namespace std;
namespace tzw {

ShaderProgram::ShaderProgram(const char *pVSFileName, const char *pFSFileName, const char *pTCSFileName, const char* pTESFileName)
{

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
    glUseProgram(shader);

}

int ShaderProgram::getShaderId()
{
    return this->shader;
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
        auto location = glGetAttribLocation(shader,name.c_str());
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

int ShaderProgram::uniformLocation(std::string name)
{
	auto iter = m_uniformMap.find(name);
	if(iter != m_uniformMap.end())
	{
		return iter->second;
	}else
	{
		int ptr =glGetUniformLocation(this->shader,name.c_str());
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
	createShader(false);
	if(shader)
	{
		glDeleteProgram(oldShader);
	}
}

void ShaderProgram::createShader(bool isStrict)
{
    this->shader = glCreateProgram();
    if (shader == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }
    tzw::Data vs_data = tzw::Tfile::shared()->getData(m_vertexShader,true);
    tzw::Data fs_data = tzw::Tfile::shared()->getData(m_fragmentShader,true);
	tlog("compiling... %s %s", m_vertexShader.c_str(), m_fragmentShader.c_str());
    if (vs_data.isNull()) {
        exit(1);
    };

    if (fs_data.isNull()) {
        exit(1);
    };

    string vs = vs_data.getString();
    string fs = fs_data.getString();
    addShader(shader, vs.c_str(), GL_VERTEX_SHADER, isStrict);
    addShader(shader, fs.c_str(), GL_FRAGMENT_SHADER, isStrict);

    if(!m_tessellationControlShader.empty())
    {
        tzw::Data tcs_data = tzw::Tfile::shared()->getData(m_tessellationControlShader,true);
        addShader(shader, tcs_data.getString().c_str(), GL_TESS_CONTROL_SHADER, isStrict);
    }

    if(!m_tessellationEvaluationShader.empty())
    {
        tzw::Data tes_data = tzw::Tfile::shared()->getData(m_tessellationEvaluationShader,true);
        addShader(shader, tes_data.getString().c_str(), GL_TESS_EVALUATION_SHADER, isStrict);
    }

    GLint Success = 0;
    GLchar ErrorLog[2048] = { 0 };
    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(shader, sizeof(ErrorLog), nullptr, ErrorLog);
        fprintf(stdout, "Error linking shader program: '%s'\n", ErrorLog);
    	if(isStrict)
    	{
    		exit(1);
    	}
        
    }

    glValidateProgram(shader);
    glGetProgramiv(shader, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(shader, sizeof(ErrorLog), nullptr, ErrorLog);
        fprintf(stdout, "Invalid shader program: '%s'\n", ErrorLog);
       	if(isStrict)
    	{
        exit(1);
        }
    }
}

void ShaderProgram::addShader(unsigned int ShaderProgram, const char *pShaderText, unsigned int ShaderType, bool isStrict)
{
	tlog("create shader!");
    GLuint ShaderObj = glCreateShader(ShaderType);
	RenderBackEnd::shared()->selfCheck();

    if (ShaderObj == 0) {
        fprintf(stdout, "Error Compile shader type %d\n", ShaderType);
    	if(isStrict)
    	{
    		exit(0);
    	}
    }
	auto size = ShaderMgr::shared()->m_macros.size();
    GLchar** p = new GLchar*[size + 2];
	GLint* Lengths = new GLint[size + 2];
	auto glsl_ver = "#version 420\n";
	p[0] = (char *)glsl_ver;
	Lengths[0] = strlen(glsl_ver);
	int index = 1;
	for(auto i : ShaderMgr::shared()->m_macros)
	{
		char * tmpStr = (char *)malloc(128);
		sprintf(tmpStr, "#define %s %s\n", i.first.c_str(), i.second.c_str());
		p[index] = tmpStr;
		Lengths[index] = strlen(tmpStr);
		index ++;
	}
    p[size + 1] = (char *)pShaderText;
    
    Lengths[size + 1]= strlen(pShaderText);
    glShaderSource(ShaderObj, size + 2, p, Lengths);
	RenderBackEnd::shared()->selfCheck();
    glCompileShader(ShaderObj);
	RenderBackEnd::shared()->selfCheck();
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	RenderBackEnd::shared()->selfCheck();
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, nullptr, InfoLog);
        fprintf(stdout, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        if (isStrict) 
		{
          exit(1);
        }
    }
    glAttachShader(ShaderProgram, ShaderObj);
	RenderBackEnd::shared()->selfCheck();
	delete[] Lengths;
	delete[] p;
}

} // namespace tzw

