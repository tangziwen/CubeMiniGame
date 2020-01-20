#include "TextureMgr.h"
#include <utility>
namespace tzw {

TZW_SINGLETON_IMPL(TextureMgr)

Texture *TextureMgr::getByPath(std::string filePath, bool isNeedMipMap)
{
    if (filePath.empty()) return nullptr;
    auto result = m_texturePool.find(filePath);
    if(result!=m_texturePool.end())
    {
        return result->second;
    }else
    {
        Texture * tex = new Texture(filePath);
        if(isNeedMipMap)
        {
            tex->genMipMap();
        }
        m_texturePool.insert(std::make_pair(filePath,tex));
        return tex;
    }
}

Texture* TextureMgr::getByPathSimple(std::string filePath)
{
	return getByPath(filePath, false);
}

Texture* TextureMgr::getByPathAsync(std::string filePath, std::function<void (Texture *)> finishedCallBack, bool isNeedMiMap)
{
    if (filePath.empty()) 
	{
    	if(finishedCallBack)
    	{
    		finishedCallBack(nullptr);
    	}
    	return nullptr;
	}
    auto result = m_texturePool.find(filePath);
    if(result!=m_texturePool.end())
    {
    	if(finishedCallBack)
    	{
    		finishedCallBack(result->second);
    	}
    	return result->second;
    }else
    {
    	return loadAsync(filePath, finishedCallBack, isNeedMiMap);
    }
}

Texture* TextureMgr::loadAsync(std::string filePath, std::function<void(Texture*)> finishedCallBack, bool isNeedMiMap)
{
    Texture * tex = new Texture();
    auto onFinished = [this, tex, isNeedMiMap, filePath, finishedCallBack](Texture *)
    {
        if(isNeedMiMap)
        {
            tex->genMipMap();
        }
        m_texturePool.insert(std::make_pair(filePath,tex));
    	finishedCallBack(tex);
    };
    tex->loadAsync(filePath, onFinished);
	return tex;
}

Texture *TextureMgr::getByPath(std::string PosX, std::string NegX, std::string PosY,
								std::string NegY, std::string PosZ, std::string NegZ)
{
    auto result = m_texturePool.find(PosX);
    if(result!=m_texturePool.end())
    {
        return result->second;
    }else
    {
        Texture * tex = new Texture(PosX,NegX,PosY,NegY,PosZ,NegZ);
        m_texturePool.insert(std::make_pair(PosX,tex));
        return tex;
    }
}

Texture* TextureMgr::loadSingleCubeMap(std::string filePath)
{
    auto result = m_texturePool.find(filePath);
    if(result!=m_texturePool.end())
    {
        return result->second;
    }else
    {
    	//there is a bug, should be EWUDNS
        Texture * tex = new Texture(filePath, "EWDUNS");
        m_texturePool.insert(std::make_pair(filePath,tex));
        return tex;
    }
}

TextureMgr::TextureMgr()
{

}

} // namespace tzw

