#include "ScripStdLib.h"
#include "External/TUtility/TUtility.h"
#include "ScriptStruct.h"
#include <stdlib.h>
namespace tzw {

ScripStdLib::ScripStdLib()
{

}

void ScripStdLib::init(ScriptVM * vm)
{
	vm->defineFunction("print",&ScripStdLib::print);
	vm->defineFunction("malloc",&ScripStdLib::malloc_script);
	vm->defineFunction("struct",&ScripStdLib::struct_script);
	vm->defineFunction("append",&ScripStdLib::append_script);
	vm->defineFunction("get",&ScripStdLib::get_script);
	vm->defineFunction("set",&ScripStdLib::set_script);
}

void ScripStdLib::print(ScriptVM * vm)
{
	auto argCount = vm->userStackPop().i();
	bool isFirst = true;
	auto log = Tlog();
	while(argCount > 0)
	{
		auto templateStr = isFirst? "%s" : " %s";
		isFirst = false;
		printf(templateStr, vm->userStackPop().toStr().c_str());
		argCount --;
	}
	printf("\n");
}

void ScripStdLib::malloc_script(ScriptVM * vm)
{
	auto argCount = vm->userStackPop().i();
	if(argCount != 1)
	{
		printf("bad use of malloc!!!!!\n");
		exit(0);
	}
	auto ptr = malloc(vm->userStackPop().i());
	vm->userStackPush(ScriptValue(ptr));
}

void ScripStdLib::struct_script(ScriptVM * vm)
{
	auto argCount = vm->userStackPop().i();
	auto theStruct = new ScriptStruct();
	if(argCount != 0)
	{
		while(argCount > 0)
		{
			auto value = new ScriptValue();
			*value = vm->userStackPop();
			theStruct->append(value);
			argCount--;
		}
	}
	auto structValue = new ScriptValue(theStruct);
	vm->addHeapObj(theStruct);
	vm->userStackPush(structValue);
}

void ScripStdLib::append_script(ScriptVM * vm)
{
	auto argCount = vm->userStackPop().i();
	if(argCount != 2)
	{
		printf("bad use of append\n");
		exit(0);
	}
	auto obj = vm->userStackPop();
	auto element = vm->userStackPop();
	if(obj.type() == ScriptValue::Type::Ref && obj.getRefValue()->type() == ScriptValue::Type::structPtr)
	{
		auto value = new ScriptValue();
		*value = element;
		obj.getRefValue()->getStructPtr()->append(value);
		return;
	}
	printf("bad use of append\n");
	exit(0);
}

void ScripStdLib::get_script(ScriptVM * vm)
{
	auto argCount = vm->userStackPop().i();
	if(argCount != 2)
	{
		printf("bad use of get\n");
		exit(0);
	}
	auto obj = vm->userStackPop();
	auto index = vm->userStackPop().i();
	if(obj.type() == ScriptValue::Type::Ref && obj.getRefValue()->type() == ScriptValue::Type::structPtr)
	{
		vm->userStackPush(obj.getRefValue()->getStructPtr()->getByIndex(index));
		return;
	}
	printf("bad use of append\n");
	exit(0);
}

void ScripStdLib::set_script(ScriptVM * vm)
{
	auto argCount = vm->userStackPop().i();
	if(argCount != 3)
	{
		printf("bad use of get\n");
		exit(0);
	}
	auto obj = vm->userStackPop();
	auto index = vm->userStackPop().i();
	auto element = vm->userStackPop();
	if(obj.type() == ScriptValue::Type::Ref && obj.getRefValue()->type() == ScriptValue::Type::structPtr)
	{
		auto value = new ScriptValue();
		*value = element;
		obj.getRefValue()->getStructPtr()->set(index, value);
		return;
	}
	printf("bad use of append\n");
	exit(0);
}

} // namespace tzw
