#include "Log.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "Game/ConsolePanel.h"
#include <ctime>

namespace tzw
{
	static FILE * fileHandle;
	void initLogSystem()
	{
		fileHandle = fopen("./log.txt","w+");
	}

	void tlog(const char * pattern, ...)
	{
		const int MAX_BUFFER_SIZE = 128;
		static char header[MAX_BUFFER_SIZE];

		time_t t = time(NULL);
		struct tm *lt = localtime(&t);
		static char thePattern[512] = {};
		snprintf(header, MAX_BUFFER_SIZE, "[%02d/%02d/%02d %02d:%02d:%02d] ", lt->tm_mon+1, lt->tm_mday, lt->tm_year%100, lt->tm_hour, lt->tm_min, lt->tm_sec);
		memset(thePattern,0,512);
		
		memcpy(thePattern,header,strlen(header));
		strcat(thePattern,pattern);
		strcat(thePattern, "\n");
		va_list vaArg;
		va_start(vaArg,pattern);
		vfprintf(stdout,thePattern, vaArg);
		vfprintf(fileHandle, thePattern, vaArg);
		auto consolePanel = ConsolePanel::shared();
		if(consolePanel)
		{
			consolePanel->AddLog(pattern, vaArg);
		}
		va_end(vaArg);
	}

	void tlogError(const char * pattern, ...)
	{
		const int MAX_BUFFER_SIZE = 128;
		static char header[MAX_BUFFER_SIZE];

		time_t t = time(NULL);
		struct tm *lt = localtime(&t);
		static char thePattern[512] = {};
		snprintf(header, MAX_BUFFER_SIZE, "[%02d/%02d/%02d %02d:%02d:%02d][error]", lt->tm_mon+1, lt->tm_mday, lt->tm_year%100, lt->tm_hour, lt->tm_min, lt->tm_sec);
		memset(thePattern,0,512);
		
		memcpy(thePattern,header,strlen(header));
		strcat(thePattern,pattern);
		strcat(thePattern, "\n");
		va_list vaArg;
		va_start(vaArg,pattern);
		vfprintf(stdout,thePattern, vaArg);
		vfprintf(fileHandle, thePattern, vaArg);
		ConsolePanel::shared()->AddLog(thePattern, vaArg);
		va_end(vaArg);
	}

}

