#include "EngineSrc/Engine/Engine.h"
#include "Application/GameEntry.h"
#include <rapidjson/rapidjson.h>
#include "External/Lua/lua.hpp"
#include <iostream>
#include <time.h>



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
	return Engine::run(argc,argv,new GameEntry());
}