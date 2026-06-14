#pragma once
#include "Engine/EngineDef.h"
#include <string>

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
  void callFunPyVoid(const char* module, const char* functionName, Types... args);
  template<class... Types>
  bool callFunPyBool(const char* module, const char* functionName, Types... args);

  void reload();

};

template<class... Types>
void
ScriptPyMgr::callFunPyVoid(const char * moduleStr, const char* functionName, Types... args)
{
	PyObject *pName, *pModule, *pDict, *pFunc;
	PyObject *pArgs, *pValue;
	pName = PyUnicode_DecodeFSDefault(moduleStr);
	pModule = PyImport_Import(pName);
	Py_DECREF(pName);
	if (pModule == nullptr)
	{
		PyErr_Print();
		return;
	}
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
            PyErr_Print();
            fprintf(stderr,"Call failed\n");
        }
	}
	else
	{
		if (PyErr_Occurred())
		{
			PyErr_Print();
		}
		tlogError("Python function %s.%s is not callable\n", moduleStr, functionName);
	}
	Py_XDECREF(pFunc);
	Py_DECREF(pModule);
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
	if (pModule == nullptr)
	{
		PyErr_Print();
		return false;
	}
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
            PyErr_Print();
            fprintf(stderr,"Call failed\n");
        }
	}
	else
	{
		if (PyErr_Occurred())
		{
			PyErr_Print();
		}
		tlogError("Python function %s.%s is not callable\n", moduleStr, functionName);
	}
	Py_XDECREF(pFunc);
	Py_DECREF(pModule);
	return returnVal;
}
	
}
