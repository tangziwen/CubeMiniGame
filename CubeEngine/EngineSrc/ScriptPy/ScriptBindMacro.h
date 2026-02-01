#pragma once
#define BIND_BEGIN_CLASS_PY(className) pybind11::class_<className>(m, #className)
#define BIND_PROP_PY(className, FUNC) .def_readwrite(#FUNC, &className## ::##FUNC)
#define BIND_FUNC_PY(className, FUNC) .def(#FUNC, &className## ::##FUNC, pybind11::return_value_policy::reference)
#define BIND_STATIC_FUNC_PY(className, FUNC) .def_static(#FUNC, &className## ::##FUNC, pybind11::return_value_policy::reference)
#define BIND_SINGLETON_PY(className) .def_static("shared", &className## ::shared, pybind11::return_value_policy::reference)

#define BIND_MODULE_FUNC_PY(className, FUNC) m.def(#FUNC, &className## ::##FUNC, pybind11::return_value_policy::reference)
#define BIND_MODULE_CUSTOM_FUNC_PY(FUNCStr, FUNCPtr) m.def(FUNCStr, FUNCPtr, pybind11::return_value_policy::reference)
#define BIND_EMPTY_CONSTRUCT_PY .def(py::init<>())