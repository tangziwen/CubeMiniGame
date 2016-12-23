#include "ShaderProgram.h"
#include "External/TUtility/TUtility.h"
#include <string.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <stdio.h>

using namespace std;
namespace tzw {

ShaderProgram::ShaderProgram(const char *pVSFileName, const char *pFSFileName, const char *pTCSFileName, const char* pTESFileName)
{
    this->shader = glCreateProgram();
    if (shader == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }
    tzw::Data vs_data = tzw::Tfile::shared()->getData(pVSFileName,true);
    tzw::Data fs_data = tzw::Tfile::shared()->getData(pFSFileName,true);
	printf("compiling... %s %s\n", pVSFileName, pFSFileName);
    if (vs_data.isNull()) {
        exit(1);
    };

    if (fs_data.isNull()) {
        exit(1);
    };

    string vs = vs_data.getString();
    string fs = fs_data.getString();
    addShader(shader, vs.c_str(), GL_VERTEX_SHADER);
    addShader(shader, fs.c_str(), GL_FRAGMENT_SHADER);

    if(pTCSFileName)
    {
        tzw::Data tcs_data = tzw::Tfile::shared()->getData(pTCSFileName,true);
        addShader(shader, tcs_data.getString().c_str(), GL_TESS_CONTROL_SHADER);
    }

    if(pTESFileName)
    {
        tzw::Data tes_data = tzw::Tfile::shared()->getData(pTESFileName,true);
        addShader(shader, tes_data.getString().c_str(), GL_TESS_EVALUATION_SHADER);
    }

    GLint Success = 0;
    GLchar ErrorLog[2048] = { 0 };
    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(shader, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glValidateProgram(shader);
    glGetProgramiv(shader, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(shader, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }
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
        }
        else{
            //T_LOG<<"there is no uniform called:"<<str<<"in shader :"<<this->shader;
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
        m_locationMap.insert(std::make_pair(name,location));
        return location;
    }
}

void ShaderProgram::enableAttributeArray(unsigned int attributeId)
{
    glEnableVertexAttribArray(attributeId);
}

void ShaderProgram::setAttributeBuffer(int ID, int dataType, int offset, int size, int stride)
{
    glVertexAttribPointer(ID,size,dataType,GL_FALSE,stride,(void *)offset);
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
		m_uniformMap[name] = ptr;
		return ptr;
	}
}

void ShaderProgram::addShader(unsigned int ShaderProgram, const char *pShaderText, unsigned int ShaderType)
{
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }

    const GLchar* p[1];
    p[0] = pShaderText;
    GLint Lengths[1];
    Lengths[0]= strlen(pShaderText);
    glShaderSource(ShaderObj, 1, p, Lengths);
    glCompileShader(ShaderObj);
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }
    glAttachShader(ShaderProgram, ShaderObj);
}

} // namespace tzw

