#pragma once
namespace tzw
{
	void TAssert_imp(bool condition, const char * assert_str, const char * fileName, int linenum, const char * outputStr, ...);
}
#ifdef TZW_STATIC_ASSERT

#define TAssert(condition) if(!condition) printf("assert %s in %s in %d\n",#condition, __FILE__, __LINE__),abort()

#else

#define TAssert(condition, outputString, ...) TAssert_imp(condition, #condition, __FILE__, __LINE__, outputString, __VA_ARGS__)

#endif