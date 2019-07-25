#pragma once

//32char uuid + 4 hyphens + 1EOS
#define TZW_UUID_LEN 37
namespace tzw {
	class GuidObj
	{
	public:
		GuidObj();
		virtual char * getGUID();
		virtual char * genGUID();
		virtual void setGUID(const char * newUUID);
		virtual ~GuidObj();
	protected:
		char m_uuid[TZW_UUID_LEN];
	};
} // namespace tzw

