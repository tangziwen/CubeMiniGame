#include <stdio.h>

#include "EngineSrc/Engine/Engine.h"
#include "Application/GameEntry.h"
#include "EngineSrc/Script/ScriptVM.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include "External/Lua/lua.hpp"
#include <iostream>
using namespace std;
using namespace tzw;

#if _MSC_VER >= 1900
FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}
#endif

#pragma comment(linker, "/subsystem:console")
int main(int argc, char *argv[])
{
	lua_State *L = luaL_newstate();
	luaopen_base(L);
	luaopen_table(L);
	luaopen_package(L);
	luaopen_io(L);
	luaopen_string(L);
	luaL_openlibs(L);
	string str;
	luaL_dofile(L,"./Entry.lua");
	lua_close(L);

	//	ScriptVM vm;
	//	vm.defineFunction("tmpFunc",tmpFunc);
	//	vm.useStdLibrary();
	//	vm.loadFromFile("./test.txt",0);
	//	vm.excute(0);
	//	return 0;
	return Engine::run(argc,argv,new GameEntry());
}

