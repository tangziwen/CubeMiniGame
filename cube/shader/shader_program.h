#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <QOpenGLFunctions_3_0>
class ShaderProgram : protected QOpenGLFunctions_3_0
{
public:
    ShaderProgram(const char * pVSFileName, const char * pFSFileName);
    void use();
    int getShaderId();
    void setUniformInteger(const char * str,int value);
    void setUniformMat4v(const char * str, const GLfloat *array, bool transpose = false, int count = 1);
    void setUniformFloat(const char * str,float value);
    void setUniform3Float(const char * str,float x,float y,float z);
    void setUniform2Float(const char * str,float x,float y);
private:
    void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
    GLuint shader;
};
#endif // SHADER_PROGRAM_H
