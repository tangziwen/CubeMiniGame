# -*- coding:utf-8 -*-
import sys
import time
import os
import importlib
g_ignoreModuleName = {"Reloader"}
g_currModules = None
g_lastReloadTime = time.time()
def getNeedToReloadModuleNames():
	global g_currModules
	g_currModules = dict(sys.modules)
	allModuleNameList = g_currModules.keys()
	resultList = []
	for name in allModuleNameList:
		if name in g_ignoreModuleName:#ignore some code
			continue
		currModule = g_currModules[name]
		if getattr(currModule, '__file__', None):
			if os.stat(currModule.__file__).st_mtime > g_lastReloadTime:
				resultList.append(name)
	return resultList

def reloadScript():
	global g_lastReloadTime
	global g_currModules
	modules = getNeedToReloadModuleNames()
	if len(modules) > 0:
		for name in modules:
			importlib.reload(g_currModules[name])
		g_lastReloadTime = time.time()
	else:
		print("nothing to reload")