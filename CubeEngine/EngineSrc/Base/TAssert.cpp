#include "TAssert.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
namespace tzw
{
	void TAssert_imp(bool condition, const char * assert_str, const char * fileName, int linenum, const char * outputStr,  ...)
	{
		if(!condition) 
		{
			char thePattern[512] = {};
			va_list vaArg;
			memcpy(thePattern,outputStr,strlen(outputStr));
			strcat(thePattern, "\n");
			va_start(vaArg,outputStr);
			fprintf(stdout, "assert!\n");
			vfprintf(stdout,thePattern, vaArg);
			fprintf(stdout, "condition : %s [%s ln: %d]\n",assert_str, fileName, linenum);
			abort();
		}
	}
} // namespace tzw

