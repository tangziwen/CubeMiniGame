#include "Log.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "Game/ConsolePanel.h"
namespace tzw
{
	void tlog(const char * pattern, ...)
	{
		static char thePattern[256] = {};
		memset(thePattern,0,256);
		memcpy(thePattern,pattern,strlen(pattern));
		strcat(thePattern, "\n");
		va_list vaArg;
		va_start(vaArg,pattern);
		vfprintf(stdout,thePattern, vaArg);
		ConsolePanel::shared()->AddLog(pattern, vaArg);
		va_end(vaArg);
	}

	void tlogError(const char * pattern, ...)
	{
		static char thePattern[256] = {};
		memset(thePattern, 0, 256);
		memcpy(thePattern, pattern, strlen(pattern));
		strcat(thePattern, "\n");
		va_list vaArg;
		va_start(vaArg, pattern);
		vfprintf(stdout, thePattern, vaArg);
		ConsolePanel::shared()->AddLog(pattern, vaArg);
		va_end(vaArg);
	}

}

