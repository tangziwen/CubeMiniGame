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
#include "Utility/file/Tfile.h"
#include "Base/BlackBoard.h"

#pragma comment(lib, "dbghelp.lib")

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

#pragma comment(linker, "/subsystem:console")

#define macro_test(A,B,C) printf("hehehe" #A #B #C );
#define TEST_VULKAN_ENTRY
#define TEST_SCRIPT 0




int main(int argc, char *argv[]) 
{
    Engine::preSetting();
	std::vector<void * > test_list = {};
	printf("%d",  test_list.size());
#if TEST_SCRIPT
	auto data = Tfile::shared()->getData("TinaTest/test.tina", true);
	TinaTokenizer *tokenizer = new TinaTokenizer();
	tokenizer->loadStr(data.getString());
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
	runtime->execute(&program, "main");

	return 0;
#endif

	return Engine::run(argc,argv,new GameEntry());
	//return Engine::run(argc,argv,new PTMEntry());

#ifdef  TEST_VULKAN_ENTRY
    return Engine::run(argc,argv,new TestVulkanEntry());
#else
    return Engine::run(argc,argv,new GameEntry());
#endif //  TEST_VULKAN_ENTRY

	
}