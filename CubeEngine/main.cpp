#include "EngineSrc/Engine/Engine.h"
#include "Application/GameEntry.h"
#include <rapidjson/rapidjson.h>
#include "External/Lua/lua.hpp"
#include <iostream>
#include <time.h>
#include <windows.h>
#include <DbgHelp.h>
#include "Math/vec3.h"
#include "Tina/TinaParser.h"
#include "Tina/TinaRunTime.h"
#pragma comment(lib, "dbghelp.lib")  

void CreateDumpFile(const TCHAR *lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)
{
    // ����Dump�ļ�
    HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // Dump��Ϣ
    //
    MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
    dumpInfo.ExceptionPointers = pException;
    dumpInfo.ThreadId = GetCurrentThreadId();
    dumpInfo.ClientPointers = TRUE;

    // д��Dump�ļ�����
    //
    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);

    CloseHandle(hDumpFile);
}
// ����Unhandled Exception�Ļص�����
//
LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
    // ��������Ӵ�������������Ĵ���
    // ���ںܶ�������ǵ���һ�����ʹ��󱨸�ĶԻ���

    // �����Ե���һ������Ի����˳�����Ϊ����
    CreateDumpFile(TEXT("last.dmp"), pException);
    FatalAppExit(-1, TEXT("Sorry, it crashed, please send the dump file to me(tzwtangziwen@163.com)"));

    return EXCEPTION_EXECUTE_HANDLER;
}

using namespace std;
using namespace tzw;
#if _MSC_VER >= 1900
FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE * __cdecl __iob_func()
{
	return _iob;
}
#endif

//Nvidia
extern "C" 
{
  __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
//AMD
extern "C"
{
  __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
	// static SoLoud::Soloud gSoloud; // SoLoud engine
	// static SoLoud::Wav gWave;      // One wave file
#pragma comment(linker, "/subsystem:console")

#define TEST_VULKAN_ENTRY
int main(int argc, char *argv[]) 
{


	TinaTokenizer *tokenizer = new TinaTokenizer();
	tokenizer->loadStr("{local a,b; b= 2; a = (5 + b) * 2; print a;}");
	std::vector<TokenInfo> result =  tokenizer->getTokenList();
	for(TokenInfo& token : result)
	{
		token.print();
	}
	TinaParser parser;
	parser.parse(result);

	TinaCompiler * compiler = new TinaCompiler();
	TinaProgram program = compiler->gen(parser.getRoot());

	TinaRunTime * runtime = new TinaRunTime();
	runtime->execute(&program);

	return 0;
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
#ifdef  TEST_VULKAN_ENTRY
    return Engine::run(argc,argv,new TestVulkanEntry());
#else
    return Engine::run(argc,argv,new GameEntry());
#endif //  TEST_VULKAN_ENTRY

	
}