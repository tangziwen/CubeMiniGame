#include "ScriptPyMgr.h"

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

namespace tzw
{
	TZW_SINGLETON_IMPL(ScriptPyMgr);
	PyObject * g_mainModule = nullptr;
	ScriptPyMgr::ScriptPyMgr()
	{

	}

	void ScriptPyMgr::init()
	{
		Py_Initialize();
		initBuildInModule();
		g_mainModule = PyImport_Import(PyString_FromString("tzw"));
	}

	void ScriptPyMgr::finalize()
	{
		Py_Finalize();
	}

	void ScriptPyMgr::doScriptInit()
	{
		if (g_mainModule != NULL) {
			auto pFunc = PyObject_GetAttrString(g_mainModule, "init");
			auto pArgs = PyTuple_New(0);
			PyObject_CallObject(pFunc, pArgs);
		}
	}

	void ScriptPyMgr::doScriptUpdate()
	{
		if (g_mainModule != NULL) {
			auto pFunc = PyObject_GetAttrString(g_mainModule, "update");
			auto pArgs = PyTuple_New(0);
			PyObject_CallObject(pFunc, pArgs);
		}
	}



	static int ConvertResult(PyObject *presult, const char *result_format, void *result)
	{
		if (!presult)
		{
			if (PyErr_Occurred())
			{
				//pydebug::PythonError();
				return -1;
			}
			return 0;
		}

		if (!result)
		{
			Py_DECREF(presult);
			return 0;
		}

		if (!PyArg_Parse(presult, const_cast<char *>(result_format), result))
		{
			//pydebug::PythonError();
			Py_DECREF(presult);
			return -1;
		}

		if (strcmp(result_format, "O") != 0)
		{
			Py_DECREF(presult);
		}

		return 0;
	}

	int ScriptPyMgr::XRunFunction(const char *module, const char *function, const char *result_format, void *result, const char *args_format, ...)
	{
		PyObject *pmodule, *pfunction, *args, *presult;

		pmodule = PyImport_ImportModule(const_cast<char *>(module));
		if (!pmodule)
		{
			PyObject *type = PyErr_Occurred();
			if (type == PyExc_NameError)
			{
				PyErr_Clear();
				return 0;
			}
			
			//PythonError();
			return -1;
		}

		pfunction = PyObject_GetAttrString(pmodule, const_cast<char *>(function));
		Py_DECREF(pmodule);
		if (!pfunction)
		{
			PyObject *type = PyErr_Occurred();
			if (type == PyExc_AttributeError)
			{
				PyErr_Clear();
				return 0;
			}

			//PythonError();
			return -2;
		}

		if (pfunction == Py_None)
		{
			return 0;
		}

		va_list args_list;
		va_start(args_list, args_format);
		args = Py_VaBuildValue(const_cast<char *>(args_format), args_list);
		va_end(args_list);

		if (!args)
		{
			Py_DECREF(pfunction);
			return -3;
		}

		presult = PyObject_CallObject(pfunction, args);
		if (presult == 0)
		{
			//PythonError();
			Py_XDECREF(pfunction);
			Py_XDECREF(args);
			return -1;
		}

		Py_XDECREF(pfunction);
		Py_XDECREF(args);


		return ConvertResult(presult, result_format, result);
	}

	static PyObject* py_haha(PyObject* self, PyObject* args)
	{
		printf("the hahah\n");
		Py_RETURN_NONE;
	}
	static PyMethodDef module_methods[] = {
		{ "haha", py_haha, METH_VARARGS, "" },
		{ NULL }
	};
	void ScriptPyMgr::initBuildInModule()
	{
		PyObject *module = Py_InitModule3("g37misc", module_methods, "g37misc module");
	}
}