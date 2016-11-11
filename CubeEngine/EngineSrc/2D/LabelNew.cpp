#include "LabelNew.h"
#include "../Rendering/Renderer.h"
#include "../Scene/SceneMgr.h"
#include "../Font/FontMgr.h"
#include "External/TUtility/TUtility.h"
namespace tzw {

LabelNew::LabelNew():
    m_atlas(nullptr)
{
    m_mesh = new Mesh();
    m_material->initFromEffect("Simple");
    m_material->setVar("color",getUniformColor());
    setCamera(g_GetCurrScene()->defaultGUICamera());
}

LabelNew *LabelNew::create(std::string theString)
{
    return LabelNew::create(theString,FontMgr::shared()->getDefaultFont());
}

LabelNew *LabelNew::create(std::string theString, Font *font)
{
    auto obj = new LabelNew();
    obj->m_string = theString;
    obj->m_font = font;
    obj->genMesh();
    return obj;
}

LabelNew *LabelNew::create(Font *font)
{
    auto obj = new LabelNew();
    obj->m_string = " ";
    obj->m_font = font;
    return obj;
}

std::string LabelNew::getString() const
{
    return m_string;
}

void LabelNew::setString(const std::string &string)
{
    m_string = string;
    genMesh();
}

Font *LabelNew::getFont() const
{
    return m_font;
}

void LabelNew::setFont(Font *font)
{
    m_font = font;
    genMesh();
}

void LabelNew::genMesh()
{
    initAtlas();
    m_mesh->clear();
    //索引可以通用,我们写在循环外
	unsigned short indices[] = {
         0,1,2,  0,2,3,
    };
    int penX = 0,penY =0;
    int preMaxWidth = 0;
    auto wideCharStr = tzw::Tmisc::StringToWString(m_string);
    for(auto c:wideCharStr)
    {
        auto charMesh = new Mesh();
        auto gNode = m_atlas->getNode(c);
//        if (!gNode) continue;
        float w = gNode->m_data.width;
        float h = gNode->m_data.rows;
        //因为所有字都被batch到一张纹理上了,所以每个子mesh的实际UV坐标都不同,
        //顶点数据需要循环设置
        VertexData vertices[] = {
            // front
            VertexData(vec3(0, 0,  -1.0f), gNode->UV(0.0f, 0.0f)),  // v0
            VertexData(vec3( w,  0,  -1.0f), gNode->UV(1.0f, 0.0f)), // v1
            VertexData(vec3( w,  h,  -1.0f), gNode->UV(1.0f, 1.0f)),  // v2
            VertexData(vec3( 0, h,  -1.0f), gNode->UV(0.0f, 1.0f)), // v3
        };
        //有些字如g,y等会下沉,需要用rows和top做个差值来移动字符位置
        int diff = - int(gNode->m_data.rows) + int(gNode->m_data.top);
        charMesh->addVertices(vertices,sizeof(vertices)/sizeof(VertexData));
        charMesh->addIndices(indices,sizeof(indices)/sizeof(unsigned short));
        charMesh->finish(false);//we don't pass it to the GPU
        Matrix44 mat;
        mat.setToIdentity();
        mat.translate(vec3(penX,penY + diff,0));
        m_mesh->merge(charMesh,mat);
        penX+= gNode->m_data.advance;
        delete charMesh;
        if(c =='\n')
        {
            preMaxWidth = std::max(penX,preMaxWidth);
            penY  -= m_font->getFontSize() + 10;
            penX = 0;
        }
    }
    int result_width = std::max(penX,preMaxWidth);
    setContentSize(vec2(result_width,abs(penY) + m_font->getFontSize()));
    m_mesh->finish();
}

void LabelNew::submitDrawCmd()
{
    m_material->setVar("color", getUniformColor());
    m_material->setTex("MainTexture",m_atlas->texture());
    RenderCommand command(m_mesh,m_material,RenderCommand::RenderType::GUI);
    setUpTransFormation(command.m_transInfo);
    command.setZorder(m_globalPiority);
    Renderer::shared()->addRenderCommand(command);
}

void LabelNew::initAtlas()
{
    if(!m_atlas)
        m_atlas = new GlyphAtlas();
    m_atlas->cleanUp();
    auto wideCharStr = tzw::Tmisc::StringToWString(m_string);
    for(auto c:wideCharStr)
    {
        m_atlas->addGlyphData(m_font->getGlyphDataFromChar(c));
    }
    m_atlas->generate();
    m_atlas->generateGLTexture();
}

} // namespace tzw
