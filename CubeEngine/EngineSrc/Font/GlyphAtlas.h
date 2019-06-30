#ifndef TZW_GLYPHATLAS_H
#define TZW_GLYPHATLAS_H

#include "GlyphData.h"
#include "../Texture/Texture.h"
#include <vector>
#include <map>
namespace tzw {
class GlyphAtlas;
struct GlyphAtlasNode{
    GlyphAtlasNode();
    GlyphData m_data;
	int m_x;
	int m_y;
	vec2 UV(float u,float v);
    GlyphAtlas * parent;
    float U(float fakeU);
    float V(float fakeV);
};

class GlyphAtlas
{
public:
    GlyphAtlas();
    void addGlyphData(GlyphData data);
    void generate();
    void test();
    void generateGLTexture();
    int height() const;
    Texture *texture() const;
    GlyphAtlasNode * getNode(unsigned long c);
    void cleanUp();
private:
    std::vector<GlyphData > m_glyphList;
    Texture * m_texture;
    std::map<unsigned long ,GlyphAtlasNode *>m_glyphMap;
    unsigned char * m_buffer;
    int m_height;
    unsigned int cellWidth,cellHeight;
};

} // namespace tzw

#endif // TZW_GLYPHATLAS_H
