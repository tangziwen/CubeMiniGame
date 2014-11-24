#include "texture.h"

Texture::Texture(const char *texture_file_name)
{
    initializeOpenGLFunctions();
    this->texture_id = SOIL_load_OGL_texture(texture_file_name,4,0, SOIL_FLAG_POWER_OF_TWO);
    initTexture();
}

int Texture::getTextureID()
{
    return this->texture_id;
}

void Texture::setTextureID(unsigned int new_id)
{
    this->texture_id = new_id;
}

void Texture::initTexture()
{
    glBindTexture(GL_TEXTURE_2D,texture_id);
    // Set nearest filtering mode for texture minification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Set bilinear filtering mode for texture magnification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
