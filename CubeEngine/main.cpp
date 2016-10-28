#include <stdio.h>

#include "EngineSrc/Engine/Engine.h"
#include "Application/GameEntry.h"
#include "EngineSrc/Script/ScriptVM.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

using namespace tzw;
void tmpFunc(ScriptVM * vm)
{

	int argCount = vm->userStackPop().i();
	int result = 0;
	while(argCount > 0)
	{
		result += vm->userStackPop().i();
		argCount --;
	}
	vm->userStackPush(ScriptValue(result));
}

int main(int argc, char *argv[])
{
	ScriptVM vm;
	vm.defineFunction("tmpFunc",tmpFunc);
	vm.useStdLibrary();
	vm.loadFromFile("./test.txt",0);
	vm.excute(0);
	return 0;
	//return Engine::run(argc,argv,new GameEntry());
}

