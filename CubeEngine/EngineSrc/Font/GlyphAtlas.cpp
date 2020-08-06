#include "GlyphAtlas.h"
#include <stdlib.h>
#include <cassert>
#include "Utility/math/TbaseMath.h"

const int DATA_ROW = 10;
#include <stdio.h>
#include <cmath>
#include <algorithm>
namespace tzw {

GlyphAtlas::GlyphAtlas(): m_height(0), cellWidth(0), cellHeight(0)
{
	m_buffer = nullptr;
	m_texture = nullptr;
}

void GlyphAtlas::addGlyphData(GlyphData data)
{
    m_glyphList.push_back(data);

}

void GlyphAtlas::generate()
{
	if(m_glyphList.empty()) return;
    cellWidth = m_glyphList.front().width;
    cellHeight = m_glyphList.front().rows;
    for (int i = 0;i<m_glyphList.size();i++)
    {
        auto glyphData = m_glyphList[i];
        cellWidth = std::max(glyphData.width,cellWidth);
        cellHeight = std::max(glyphData.rows,cellHeight);
    }
    int theCellSize = m_glyphList.size();
    m_height = std::ceil(1.0 * theCellSize / DATA_ROW);
    //有可能不是pot的所以需要一些调整
    //申请一块足够放下所有GlyphData的glbuffer的(且是方形的)空间
    auto buffSize = (m_height * cellHeight) * (DATA_ROW * cellWidth) * sizeof(unsigned char);
    m_buffer = static_cast<unsigned char *>(malloc(buffSize));
	assert(m_buffer);
    memset(m_buffer,0,buffSize);
    for (int i = 0;i<m_glyphList.size();i++)
    {


        auto glyphData = m_glyphList[i];
        auto destCellY = i/DATA_ROW;
        auto destCellX = i %DATA_ROW;
        auto offset = (destCellX)*cellWidth +(destCellY) *DATA_ROW * (cellWidth * cellHeight);
        for (int y = 0; y<glyphData.rows;y++)
        {
            for(int x = 0; x<glyphData.width;x++)
            {

                m_buffer[ offset +  (y * DATA_ROW * cellWidth + x)] = glyphData.buffer[y * glyphData.width + x];
            }
        }
        auto node =new GlyphAtlasNode();
        node->parent = this;
        node->m_data = glyphData;
        node->m_x = destCellX * cellWidth;
        node->m_y = destCellY * cellHeight;
        m_glyphMap[glyphData.m_char] = node;
    }
}

void GlyphAtlas::test()
{
    auto file = fopen("./glyph_atlas_test.txt","w");
    for(int j = 0 ;j<cellHeight * m_height;j++)
    {
        for(int i =0;i<cellWidth * DATA_ROW;i++)
        {
            if(m_buffer[j * cellWidth * DATA_ROW  + i])
            {
                fprintf(file,"#");
            }else
            {
                fprintf(file," ");
            }
        }
        fprintf(file,"\n");
    }
    fclose(file);
}

void GlyphAtlas::generateGLTexture()
{
	if(m_glyphList.empty()) return;
    auto byteWidth = DATA_ROW * cellWidth;
    auto byteHeight = m_height *cellHeight;
    auto height_pow = TbaseMath::nextPow2(byteHeight);
    auto width_pow = TbaseMath::nextPow2(byteWidth);

    //expand the data to opengl compactable:
    auto glBuffer = new unsigned char[ 2 * width_pow * height_pow];
    for(int j=0; j <height_pow;j++) {
        for(int i=0; i < width_pow; i++){
			if(i>=byteWidth || j>=byteHeight)
			{
				glBuffer[2*(i+j*width_pow)+1] = 0;

			}else
			{
				glBuffer[2*(i+j*width_pow)+1] = m_buffer[i + byteWidth*j];
			}
            glBuffer[2*(i+j*width_pow)] = glBuffer[2*(i+j*width_pow)+1];
        }
    }
    m_texture= new Texture(glBuffer,width_pow,height_pow, ImageFormat::R8G8);
    delete [] glBuffer;
}

int GlyphAtlas::height() const
{
    return m_height;
}

Texture *GlyphAtlas::texture() const
{
    return m_texture;
}

GlyphAtlasNode *GlyphAtlas::getNode(unsigned long c)
{
    return m_glyphMap[c];
}

void GlyphAtlas::cleanUp()
{
    m_glyphMap.clear();
    m_glyphList.clear();
    if(m_buffer)
    {
        free(m_buffer);
		m_buffer = nullptr;
    }
}

GlyphAtlasNode::GlyphAtlasNode(): m_data(), m_x(0), m_y(0), parent(nullptr)
{
}

vec2 GlyphAtlasNode::UV(float u, float v)
{
    return vec2(U(u),V(v));
}

float GlyphAtlasNode::U(float fakeU)
{
    auto texSize = parent->texture()->getSize();
    int pos = m_x + m_data.width*fakeU;
    return 1.0f*pos / texSize.x;
}

float GlyphAtlasNode::V(float fakeV)
{
    auto texSize = parent->texture()->getSize();
    fakeV = 1.0 - fakeV;
    auto theY = texSize.y - m_y;
    int pos = theY - m_data.rows*fakeV;
    return 1.0f*pos / texSize.y;
}

} // namespace tzw
