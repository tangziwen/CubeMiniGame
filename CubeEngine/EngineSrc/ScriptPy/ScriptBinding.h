#include "ScriptPyMgr.h"


#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

#include "Scene/SceneMgr.h"
#include "ScriptBase.h"
#include "Utility/log/Log.h"
#include "Engine/Engine.h"
#include "Event/EventMgr.h"

namespace tzw
{
	void g_init_engine_libs();
}
