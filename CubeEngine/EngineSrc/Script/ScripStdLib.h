#ifndef TZW_SCRIPSTDLIB_H
#define TZW_SCRIPSTDLIB_H
#include "ScriptVM.h"

namespace tzw {

class ScripStdLib
{
public:
	ScripStdLib();
	static void init(ScriptVM * vm);
	static void print(ScriptVM * vm);
	static void malloc_script(ScriptVM * vm);
	static void struct_script(ScriptVM * vm);
	static void append_script(ScriptVM * vm);
	static void get_script(ScriptVM * vm);
	static void set_script(ScriptVM * vm);
};

} // namespace tzw

#endif // TZW_SCRIPSTDLIB_H
