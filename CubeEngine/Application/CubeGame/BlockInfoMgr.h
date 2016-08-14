#ifndef TZW_BLOCKINFOMGR_H
#define TZW_BLOCKINFOMGR_H

#include <map>
#include "BlockInfo.h"

namespace tzw {

class BlockInfoMgr
{
public:
    static BlockInfoMgr * shared();
    BlockInfo * createOrGetInfo(std::string configFile);
private:
    static BlockInfoMgr * m_instance;
    std::map<std::string,BlockInfo *> m_infoMap;
    void loadInfo(BlockInfo * info,std::string configFile);
    BlockInfoMgr();
};

} // namespace tzw

#endif // TZW_BLOCKINFOMGR_H
