# -*- coding:utf-8 -*-
import CubeEngine
import Main
import Reloader
print("love from Cube Engine")
a = 3
print("aaaaa", a)
def tzw_engine_init():
	print("tzw_engine_init from Python")
	Main.onEngineInit()

def tzw_engine_ui_update(dt):
	Main.handleUIUpdate(dt)
	
def tzw_engine_input_event(eventInfo):
	Main.onEngineInputEvent(eventInfo)

def tzw_on_game_ready():
	print("on Game Ready")
	pass

def tzw_on_gm_command(commandStr):
	print("the GM command str is", commandStr)
	if commandStr == "r":
		print("reload module")
		Reloader.reloadScript()
		pass

#specified window draw
def tzw_game_draw_window(arg):

	return Main.onDrawWindow(arg)

def tzw_engine_reload():
	pass

def on_game_start():
	Main.on_game_start()