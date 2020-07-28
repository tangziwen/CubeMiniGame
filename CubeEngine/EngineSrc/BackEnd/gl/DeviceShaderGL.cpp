#include "DeviceShaderGL.h"
#include <External\GLEW\GL\glew.h>
#include <stdio.h>
#include "BackEnd/RenderBackEnd.h"
namespace tzw
{



	void DeviceShaderGL::addShader(const unsigned char* buff, size_t size, DeviceShaderType type, const unsigned char* fileInfoStr)
	{
        unsigned int glShaderType = GL_VERTEX_SHADER;
        switch(type){
        case DeviceShaderType::VertexShader:
            glShaderType = GL_VERTEX_SHADER;
        break;
        case DeviceShaderType::FragmentShader:
            glShaderType = GL_FRAGMENT_SHADER;
        break;
        case DeviceShaderType::TessControlShader:
            glShaderType = GL_TESS_CONTROL_SHADER;
        break;
        case DeviceShaderType::TessEvaulateShader:
            glShaderType = GL_TESS_EVALUATION_SHADER;
        break;
        }
        GLuint ShaderObj = glCreateShader(glShaderType);
        if (ShaderObj == 0) {
            fprintf(stdout, "Error Compile shader type %d\n", glShaderType);
        }
	    RenderBackEnd::shared()->selfCheck();
        GLchar * shaderArray[] = {(GLchar *)buff};
        GLint shaderSizeArray[] = {size};
        glShaderSource(ShaderObj, 1, shaderArray, shaderSizeArray);
	    RenderBackEnd::shared()->selfCheck();
        glCompileShader(ShaderObj);
	    RenderBackEnd::shared()->selfCheck();
        GLint success;
        glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	    RenderBackEnd::shared()->selfCheck();
        if (!success) {
            GLchar InfoLog[1024];
            glGetShaderInfoLog(ShaderObj, 1024, nullptr, InfoLog);
            fprintf(stdout, "Error compiling shader type %d: '%s'\n", glShaderType, InfoLog);
        }
        glAttachShader(m_uid, ShaderObj);
	    RenderBackEnd::shared()->selfCheck();
	}

	bool DeviceShaderGL::create()
	{
		m_uid = glCreateProgram();
		return m_uid != 0;
	}

    bool DeviceShaderGL::finish()
    {
        bool isOk = true;
        GLint Success = 0;
        GLchar ErrorLog[2048] = { 0 };
        glLinkProgram(m_uid);
        glGetProgramiv(m_uid, GL_LINK_STATUS, &Success);
        if (Success == 0) {
            glGetProgramInfoLog(m_uid, sizeof(ErrorLog), nullptr, ErrorLog);
            fprintf(stdout, "Error linking shader program: '%s'\n", ErrorLog);
            isOk = false;
        
        }

        glValidateProgram(m_uid);
        glGetProgramiv(m_uid, GL_VALIDATE_STATUS, &Success);
        if (!Success) {
            glGetProgramInfoLog(m_uid, sizeof(ErrorLog), nullptr, ErrorLog);
            fprintf(stdout, "Invalid shader program: '%s'\n", ErrorLog);
            isOk = false;
        }
        return isOk;
    }

}