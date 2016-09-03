#ifndef TZW_BLOCK_H
#define TZW_BLOCK_H

#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Texture/TextureAtlas.h"
#include "./BlockInfoMgr.h"
#define BLOCK_FACE_TOP 0
#define BLOCK_FACE_BOTTOM 1
#define BLOCK_FACE_SIDE 2
#define BLOCK_SIZE 1.0
namespace tzw {

class Block : public Drawable3D
{
public:
    Block();
    static Block* create(std::string configFile);
    int grid_x,grid_y,grid_z;
    void init();
    BlockInfo *info() const;
    int localGridX,localGridY,localGridZ;
private:
    BlockInfo * m_info;
};

} // namespace tzw

#endif // TZW_BLOCK_H
