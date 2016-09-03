#include "Block.h"


namespace tzw {

Block::Block()
{
}

Block *Block::create(std::string configFile)
{
    auto newBlock =  new Block();
    newBlock->m_info = BlockInfoMgr::shared()->createOrGetInfo(configFile);
    newBlock->init();
    return newBlock;
}


void Block::init()
{
    m_localAABB.merge(m_info->mesh()->getAabb());
    setIsAccpectOCTtree(false);
}

BlockInfo *Block::info() const
{
    return m_info;
}

} // namespace tzw

