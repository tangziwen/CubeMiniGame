#pragma once
#include "Engine/EngineDef.h"
#include <string>

#include "Lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "Utility/log/Log.h"
#include "Python.h"
#include "Base/TypeTraits.h"
namespace tzw
{
class EventInfo;
class ScriptPyMgr : public Singleton<ScriptPyMgr>
{
public:
  ScriptPyMgr();
  void init();
  void doScriptInit();
  void doScriptUIUpdate();
  void finalize();
  void raiseInputEvent(EventInfo eventInfo);

  void pushArg() {}
  template<typename T, class... Types>
  void pushArg(T t, Types... args)
  {
    luabridge::push(g_lua_state, t);
    pushArg(args...);
  }

  template<typename T>
	PyObject *  genPyObject(T t)
  {
	PyObject *pValue;
	if constexpr(isInt<T>::value)
	{
	pValue = PyLong_FromLong(t);
	}
	else if constexpr(isBool<T>::value)
	{
	pValue = PyBool_FromLong(t);
	}
	else if constexpr(isFloat<T>::value)
	{
	pValue = PyFloat_FromDouble(t);
	}
	else if constexpr(isString<T>::value)
	{
	if constexpr (isCstring<T>::value)//is It a C String?
	{
  		pValue = PyUnicode_FromString(t);
	}else
	{
  		pValue = PyUnicode_FromString(t.c_str());
	}
	}
	else//defalut
	{
		pValue = PyLong_FromLong(0);
	}
  	return pValue;
  }

  inline void pushArgPy(PyObject *argTuple, int index)
  {
  }

  template<typename T, class... Types>
  void pushArgPy(PyObject *argTuple, const int index, T t, Types... args)
  {
  	PyObject *pValue = genPyObject(t);
    /* pValue reference stolen here: */
    PyTuple_SetItem(argTuple, index, pValue);
    pushArgPy(argTuple, index + 1, args...);
  }

  template<class... Types>
  void callFunVoid(const char* functionName, Types... args);

  template<class... Types>
  bool callFunBool(const char* functionName, Types... args);

  template<class... Types>
  int callFunInt(const char* functionName, Types... args);

  template<class... Types>
  void callFunPyVoid(const char* module, const char* functionName, Types... args);
  template<class... Types>
  bool callFunPyBool(const char* module, const char* functionName, Types... args);

  std::string runString(std::string theStr);
  void reload();
  void* getState();
  void doString(std::string);

private:
  lua_State* g_lua_state;
};

template<class... Types>
void
ScriptPyMgr::callFunVoid(const char* functionName, Types... args)
{
  lua_getglobal(g_lua_state, functionName);
  size_t size = sizeof...(args);
  pushArg(args...);
  if (lua_pcall(g_lua_state, size, 0, 0) != 0) {
    tlogError("error : %s\n", lua_tostring(g_lua_state, -1));
  }
}

template<class... Types>
bool
ScriptPyMgr::callFunBool(const char* functionName, Types... args)
{
  lua_getglobal(g_lua_state, functionName);
  size_t size = sizeof...(args);
  pushArg(args...);
  if (lua_pcall(g_lua_state, size, 1, 0) != 0) {
    tlogError("error : %s\n", lua_tostring(g_lua_state, -1));
  }
  bool returnVal = lua_toboolean(g_lua_state, -1);
  lua_pop(g_lua_state, 1);
  return returnVal;
}

template<class... Types>
int
ScriptPyMgr::callFunInt(const char* functionName, Types... args)
{
  lua_getglobal(g_lua_state, functionName);
  size_t size = sizeof...(args);
  pushArg(args...);
  if (lua_pcall(g_lua_state, size, 1, 0) != 0) {
    tlogError("error : %s\n", lua_tostring(g_lua_state, -1));
  }
  int returnVal = lua_tointeger(g_lua_state, -1);
  lua_pop(g_lua_state, 1);
  return returnVal;
}

template<class... Types>
void
ScriptPyMgr::callFunPyVoid(const char * moduleStr, const char* functionName, Types... args)
{
	PyObject *pName, *pModule, *pDict, *pFunc;
	PyObject *pArgs, *pValue;
	pName = PyUnicode_DecodeFSDefault(moduleStr);
	pModule = PyImport_Import(pName);
	Py_DECREF(pName);
	pFunc = PyObject_GetAttrString(pModule, functionName);
	size_t size = sizeof...(args);

    if (pFunc && PyCallable_Check(pFunc)) 
	{
        pArgs = PyTuple_New(size);

    	pushArgPy(pArgs, 0,  args...);
        pValue = PyObject_CallObject(pFunc, pArgs);
        Py_DECREF(pArgs);
        if (pValue != NULL) {
            // printf("Result of call: %ld\n", PyLong_AsLong(pValue));
            Py_DECREF(pValue);
        }
        else {
            Py_DECREF(pFunc);
            Py_DECREF(pModule);
            PyErr_Print();
            fprintf(stderr,"Call failed\n");
        }
	}
}

template<class... Types>
bool
ScriptPyMgr::callFunPyBool(const char * moduleStr, const char* functionName, Types... args)
{
	PyObject *pName, *pModule, *pDict, *pFunc;
	PyObject *pArgs, *pValue;
	pName = PyUnicode_DecodeFSDefault(moduleStr);
	pModule = PyImport_Import(pName);
	Py_DECREF(pName);
	pFunc = PyObject_GetAttrString(pModule, functionName);
	size_t size = sizeof...(args);
	float returnVal = false;
    if (pFunc && PyCallable_Check(pFunc)) 
	{
        pArgs = PyTuple_New(size);

    	pushArgPy(pArgs, 0,  args...);
        pValue = PyObject_CallObject(pFunc, pArgs);
    	
        Py_DECREF(pArgs);
        if (pValue != NULL) {
            // printf("Result of call: %ld\n", PyLong_AsLong(pValue));
        	returnVal = (bool)PyLong_AsLong(pValue);
            Py_DECREF(pValue);
        }
        else {
            Py_DECREF(pFunc);
            Py_DECREF(pModule);
            PyErr_Print();
            fprintf(stderr,"Call failed\n");
        }
	}
	return returnVal;
}
	
}
