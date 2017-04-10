#include "Log.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

namespace tzw
{
	void log(const char * pattern, ...)
	{
		static char thePattern[256] = {};
		memset(thePattern,0,256);
		memcpy(thePattern,pattern,strlen(pattern));
		strcat(thePattern, "\n");
		va_list vaArg;
		va_start(vaArg,pattern);
		vfprintf(stdout,thePattern, vaArg);
		va_end(vaArg);
	}
}

