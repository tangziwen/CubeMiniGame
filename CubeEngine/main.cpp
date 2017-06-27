#include <stdio.h>

#include "EngineSrc/Engine/Engine.h"
#include "Application/GameEntry.h"
#include "EngineSrc/Script/ScriptVM.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include "External/Lua/lua.hpp"
#include <iostream>
#include "EngineSrc/Math/Matrix44.h"
#include <time.h>

using namespace std;
using namespace tzw;
#include "EngineSrc/Base/Log.h"
#if _MSC_VER >= 1900
FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}
#endif

#pragma comment(linker, "/subsystem:console")

void split(std::string& s, std::string delim,std::vector< std::string >* ret)
{
	size_t last = 0;
	size_t index=s.find_first_of(delim,last);
	while (index!=std::string::npos)
	{
		ret->push_back(s.substr(last,index-last));
		last=index+1;
		index=s.find_first_of(delim,last);
	}
	if (index-last>0)
	{
		ret->push_back(s.substr(last,index-last));
	}
} 

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
	srand(time(NULL));
	auto result = rand() % 100;
	printf("the result %d", result);
	lua_close(L);
	//ScriptVM vm;
	//vm.useStdLibrary();
	//vm.loadFromFile("./test.txt",0);
	//vm.excute(0);
	return Engine::run(argc,argv,new GameEntry());
}