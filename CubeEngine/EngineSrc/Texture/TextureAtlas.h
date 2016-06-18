#ifndef TZW_TEXTURESHEET_H
#define TZW_TEXTURESHEET_H
#include <string>
#include "Texture.h"
#include <map>
#include "TextureFrame.h"
namespace tzw {

class TextureAtlas
{
public:
    TextureAtlas(std::string sheetFile);
    Texture *texture() const;
    void setTexture(Texture *texture);
    int height() const;
    void setHeight(int height);

    int width() const;
    void setWidth(int width);
    TextureFrame * getFrame(std::string name);
private:
    Texture * m_texture;
    int m_width;
    int m_height;
    std::map<std::string,TextureFrame *>m_frameList;
};

} // namespace tzw

#endif // TZW_TEXTURESHEET_H
