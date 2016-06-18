#ifndef TZW_TEXTUREFRAME_H
#define TZW_TEXTUREFRAME_H
#include "Texture.h"
#include "EngineSrc/Math/vec2.h"
#include <string>
namespace tzw {
class TextureAtlas;
class TextureFrame
{
public:
    TextureFrame();
    int x,y,w,h;
    TextureAtlas *sheet() const;
    void setSheet(TextureAtlas *sheet);
    Texture * getTexture();
    std::string getName() const;
    void setName(const std::string &name);
    float U(float fakeU);
    float V(float fakeV);
    vec2 UV(float u,float v);
private:
    TextureAtlas * m_sheet;
    std::string m_name;
};

} // namespace tzw

#endif // TZW_TEXTUREFRAME_H
