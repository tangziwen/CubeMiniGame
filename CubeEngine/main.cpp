#include "EngineSrc/Engine/Engine.h"
#include "Application/GameEntry.h"
#include <rapidjson/rapidjson.h>
#include "External/Lua/lua.hpp"
#include <iostream>
#include <time.h>



#include "AudioSystem/AudioSystem.h"
#include "include/soloud_wav.h"
#include "ScriptPy/ScriptPyMgr.h"
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

		// auto result = gSoloud.init();
		// printf("backEnd %s %u", gSoloud.getBackendString(), result);
		// // Configure sound source
		// SoLoud::Wav wav; 
		// wav.load("./audio/windy_ambience.ogg");       // Load a wave file
		// wav.setLooping(1);                          // Tell SoLoud to loop the sound
		// int handle1 = gSoloud.play(wav);             // Play it
		// gSoloud.setVolume(handle1, 0.5f);            // Set volume; 1.0f is "normal"
		// gSoloud.setPan(handle1, -0.2f);              // Set pan; -1 is left, 1 is right
		// gSoloud.setRelativePlaySpeed(handle1, 0.9f); // Play a bit slower; 1.0f is normal
		AudioSystem::shared()->init();
return Engine::run(argc,argv,new GameEntry());
}