#pragma once
#include "Engine/EngineDef.h"
#include <map>

namespace tzw {
	class GUIDMgr
	{
	public:
		TZW_SINGLETON_DECL(GUIDMgr);
		void * get(std::string guid);
		void update(std::string newGUID, std::string oldGUID);
		void add(std::string guid, void * ptr);
		void remove(std::string guid);
		std::string genGUID();
		GUIDMgr();
	private:
		std::map<std::string, void *> m_map;
	};
} // namespace tzw

