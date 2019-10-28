#include "GuidMgr.h"
#include <assert.h>
extern "C"
{
#include "uuid4.h"
}

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

		auto ptr = get(oldGUID);
		if(ptr)
		{
			remove(oldGUID);
		}
		add(newGUID, ptr);
	}

	void GUIDMgr::add(std::string guid, void* ptr)
	{
		//assert(m_map.find(guid) == m_map.end());
		if(m_map.find(guid) != m_map.end())
		{
			printf("duplicated\n");
		}
		//m_map.insert(std::make_pair(guid, ptr));
		m_map[guid] = ptr;
	}

	void GUIDMgr::remove(std::string guid)
	{
		auto iter = m_map.find(guid);
		if(iter != m_map.end())
		{
			m_map.erase(iter);
		}
	}

	std::string GUIDMgr::genGUID()
	{
		char uuid[UUID4_LEN];
		uuid4_generate(uuid);
		return uuid;
	}

	GUIDMgr::GUIDMgr()
	{

	}
} // namespace tzw

