#include "GuidObj.h"
#include <string.h>
extern "C"
{
#include "uuid4.h"
}

#include "GuidMgr.h"
namespace tzw
{
	GuidObj::GuidObj()
	{
		memset(m_uuid, 0, TZW_UUID_LEN);
		GuidObj::genGUID();
		GUIDMgr::shared()->add(m_uuid, this);
	}

	char* GuidObj::getGUID()
	{
		return m_uuid;
	}

	char* GuidObj::genGUID()
	{
		uuid4_generate(m_uuid);
		return m_uuid;
	}

	void GuidObj::setGUID(const char* newUUID)
	{
		GUIDMgr::shared()->update(newUUID, m_uuid);
		strcpy(m_uuid, newUUID);
	}

    GuidObj::~GuidObj()
	{
		
	}
} // namespace tzw

