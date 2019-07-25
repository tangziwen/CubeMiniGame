#include "GuidMgr.h"
namespace tzw
{
	TZW_SINGLETON_IMPL(GUIDMgr)

	void* GUIDMgr::get(std::string guid)
	{
		auto iter = m_map.find(guid);
		if(iter != m_map.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	void GUIDMgr::update(std::string newGUID, std::string oldGUID)
	{
		if(newGUID.compare("64193964-4e4f-4bc3-9906-3939eeaed724") ==0) {
			printf("aaaaaaaa");
			}
		auto ptr = get(oldGUID);
		if(ptr)
		{
			remove(oldGUID);
		}
		add(newGUID, ptr);
	}

	void GUIDMgr::add(std::string guid, void* ptr)
	{
		m_map.insert(std::make_pair(guid, ptr));
	}

	void GUIDMgr::remove(std::string guid)
	{
		auto iter = m_map.find(guid);
		if(iter != m_map.end())
		{
			m_map.erase(iter);
		}
	}

	GUIDMgr::GUIDMgr()
	{

	}
} // namespace tzw

