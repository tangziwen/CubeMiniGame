#include <stdio.h>

#include "EngineSrc/Engine/Engine.h"
#include "Application/GameEntry.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include "External/Lua/lua.hpp"
#include <iostream>
#include "EngineSrc/Math/Matrix44.h"

#include "ScriptPy/ScriptPyMgr.h"

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

int main(int argc, char *argv[])
{
	return Engine::run(argc,argv,new GameEntry());
}

