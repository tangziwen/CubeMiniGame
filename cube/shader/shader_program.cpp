#include "shader_program.h"
#include "utility.h"
#include <QDebug>
#include <assert.h>
ShaderProgram::ShaderProgram(const char *pVSFileName, const char *pFSFileName)
{
    initializeOpenGLFunctions ();
    this->shader = glCreateProgram();

    if (shader == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    string vs, fs;

    if (!utility::ReadFile(pVSFileName, vs)) {
        exit(1);
    };

    if (!utility::ReadFile(pFSFileName, fs)) {
        exit(1);
    };

    AddShader(shader, vs.c_str(), GL_VERTEX_SHADER);
    AddShader(shader, fs.c_str(), GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };

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
    int current_shader=0;
    glGetIntegerv(GL_CURRENT_PROGRAM,&current_shader);
    if(current_shader == this->shader)
    {
        int ptr =glGetUniformLocation(this->shader,str);
        if(ptr!=-1)
        {
            glUniform1i(ptr,value);
        }
        else{
            //T_LOG<<"there is no uniform called:"<<str<<"in shader :"<<this->shader;
        }
    }else
    {
        T_LOG<<"please use this shader before set uniform";
    }
}

void ShaderProgram::setUniformMat4v(const char *str,const GLfloat *array, bool transpose, int count)
{
    int current_shader=0;
    glGetIntegerv(GL_CURRENT_PROGRAM,&current_shader);
    if(current_shader == this->shader)
    {
        int ptr =glGetUniformLocation(this->shader,str);
        if(ptr!=-1)
        {
            glUniformMatrix4fv(ptr,count,transpose,array);
        }
        else{
            //T_LOG<<"there is no uniform called:"<<str<<"in shader :"<<this->shader;
        }
    }else
    {
        T_LOG<<"please use this shader before set uniform";
    }
}

void ShaderProgram::setUniformFloat(const char *str, float value)
{
    int current_shader=0;
    glGetIntegerv(GL_CURRENT_PROGRAM,&current_shader);
    if(current_shader == this->shader)
    {
        int ptr =glGetUniformLocation(this->shader,str);
        if(ptr!=-1)
        {
            glUniform1f(ptr,value);
        }
        else{
            //T_LOG<<"there is no uniform called:"<<str<<"in shader :"<<this->shader;
        }
    }else
    {
        T_LOG<<"please use this shader before set uniform";
    }
}

void ShaderProgram::setUniform3Float(const char *str, float x, float y, float z)
{
    int current_shader=0;
    glGetIntegerv(GL_CURRENT_PROGRAM,&current_shader);
    if(current_shader == this->shader)
    {
        int ptr =glGetUniformLocation(this->shader,str);
        if(ptr!=-1)
        {
            glUniform3f(ptr,x,y,z);
        }
        else{
            //T_LOG<<"there is no uniform called:"<<str<<"in shader :"<<this->shader;
        }
    }else
    {
        T_LOG<<"please use this shader before set uniform";
    }
}

void ShaderProgram::setUniform3Float(const char *str, QVector3D v)
{
    setUniform3Float(str,v.x (),v.y (),v.z ());
}

void ShaderProgram::setUniform2Float(const char *str, float x, float y)
{
    int current_shader=0;
    glGetIntegerv(GL_CURRENT_PROGRAM,&current_shader);
    if(current_shader == this->shader)
    {
        int ptr =glGetUniformLocation(this->shader,str);
        if(ptr!=-1)
        {
            glUniform2f(ptr,x,y);
        }
        else{
            //T_LOG<<"there is no uniform called:"<<str<<"in shader :"<<this->shader;
        }
    }else
    {
        T_LOG<<"please use this shader before set uniform";
    }
}

void ShaderProgram::setUniform4Float(const char *str, float x, float y, float z, float w)
{
    int current_shader=0;
    glGetIntegerv(GL_CURRENT_PROGRAM,&current_shader);
    if(current_shader == this->shader)
    {
        int ptr =glGetUniformLocation(this->shader,str);
        if(ptr!=-1)
        {
            glUniform4f(ptr,x,y,z,w);
        }
        else{
            //T_LOG<<"there is no uniform called:"<<str<<"in shader :"<<this->shader;
        }
    }else
    {
        T_LOG<<"please use this shader before set uniform";
    }
}

void ShaderProgram::AddShader(GLuint ShaderProgram, const char *pShaderText, GLenum ShaderType)
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
