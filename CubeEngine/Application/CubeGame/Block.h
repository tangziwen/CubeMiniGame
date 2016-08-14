#ifndef TZW_BLOCK_H
#define TZW_BLOCK_H

#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Texture/TextureAtlas.h"
#include "./BlockInfoMgr.h"
#define BLOCK_FACE_TOP 0
#define BLOCK_FACE_BOTTOM 1
#define BLOCK_FACE_SIDE 2
#define BLOCK_SIZE 2
namespace tzw {

class Block : public Drawable3D
{
public:
    Block();
    static Block* create(std::string configFile);
    void setVisible(int face, bool isVisible);
    virtual void draw();
    int grid_x,grid_y,grid_z;
    void init();
    BlockInfo *info() const;
    int localGridX,localGridY,localGridZ;
private:
    Technique * m_tech;
    BlockInfo * m_info;
    bool m_faceVisibility[3];
};

} // namespace tzw

#endif // TZW_BLOCK_H
