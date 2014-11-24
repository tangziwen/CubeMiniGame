#ifndef TEXTURE_H
#define TEXTURE_H
#include "external/SOIL/SOIL.h"
#include <QOpenGLFunctions>
class Texture : protected QOpenGLFunctions
{
public:
    Texture(const char * texture_file_name);
    int getTextureID();
    void setTextureID(unsigned int new_id);
private:
    void initTexture();
    unsigned int texture_id;
};

#endif // TEXTURE_H
