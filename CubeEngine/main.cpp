#include "EngineSrc/Engine/Engine.h"
#include "Application/GameEntry.h"
#include <rapidjson/rapidjson.h>
#include "External/Lua/lua.hpp"
#include <iostream>
#include <time.h>
#include <windows.h>
#include <DbgHelp.h>  
#pragma comment(lib, "dbghelp.lib")  

void CreateDumpFile(const TCHAR *lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)
{
    // 创建Dump文件
    HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // Dump信息
    //
    MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
    dumpInfo.ExceptionPointers = pException;
    dumpInfo.ThreadId = GetCurrentThreadId();
    dumpInfo.ClientPointers = TRUE;

    // 写入Dump文件内容
    //
    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);

    CloseHandle(hDumpFile);
}
// 处理Unhandled Exception的回调函数
//
LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
    // 在这里添加处理程序崩溃情况的代码
    // 现在很多软件都是弹出一个发送错误报告的对话框

    // 这里以弹出一个错误对话框并退出程序为例子
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

int main(int argc, char *argv[]) 
{

    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
	return Engine::run(argc,argv,new GameEntry());
}