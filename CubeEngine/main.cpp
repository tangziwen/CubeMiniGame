#include <stdio.h>

#include "EngineSrc/Engine/Engine.h"
#include "Application/GameEntry.h"
#include "EngineSrc/Script/ScriptVM.h"

using namespace tzw;
int main(int argc, char *argv[])
{
//        std::string str = "var a = 105; a = a + 1; print(a);";
//        ScriptVM vm;
//        printf("calculate %s\n",str.c_str());
//        vm.loadFromStr(str,"repl");
//        vm.defineGlobal("a",ScriptValue(99));
//        vm.excute("repl");
//        return 0;
    return Engine::run(argc,argv,new GameEntry());
}
