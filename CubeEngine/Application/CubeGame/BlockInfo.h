#ifndef TZW_BLOCKINFO_H
#define TZW_BLOCKINFO_H
#include <string>

#include "EngineSrc/Texture/TextureFrame.h"
#include "EngineSrc/Mesh/Mesh.h"
namespace tzw {

class BlockInfo
{
public:
    BlockInfo();
    void finish();
    std::string topFile() const;
    void setTopFile(const std::string &topFile);

    std::string sideFile() const;
    void setSideFile(const std::string &sideFile);

    std::string bottomFile() const;
    void setBottomFile(const std::string &bottomFile);

    TextureFrame *topTexture() const;
    void setTopTexture(TextureFrame *topTexture);

    TextureFrame *sideTexture() const;
    void setSideTexture(TextureFrame *sideTexture);

    TextureFrame *bottomTexture() const;
    void setBottomTexture(TextureFrame *bottomTexture);
    void initMesh();
    Mesh *mesh() const;

private:
    std::string m_topFile;
    std::string m_sideFile;
    std::string m_bottomFile;
    TextureFrame * m_topTexture;
    TextureFrame * m_sideTexture;
    TextureFrame * m_bottomTexture;
    Mesh * m_mesh;
};

} // namespace tzw

#endif // TZW_BLOCKINFO_H
