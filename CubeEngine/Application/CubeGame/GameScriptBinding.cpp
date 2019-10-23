#include "GameScriptBinding.h"
#include "Lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "ScriptPy/ScriptPyMgr.h"
#include "CubePlayer.h"
#include "GameWorld.h"
#include "GameItem.h"
#include "BuildingSystem.h"
#include "CylinderPart.h"
#include "LiftPart.h"


#define BIND_PROP(className, PROP) .addProperty(#PROP, &className## ::##PROP)
#define BIND_FUNC(className, FUNC) .addFunction(#FUNC, &className## ::##FUNC)
#define BIND_START(state) luabridge::getGlobalNamespace(state)
#define BIND_BEGIN_CLASS(className) .beginClass<className>(#className)
#define BIND_END_CLASS .endClass();
#define BIND_EMPTY_CONSTRUCT .addConstructor <void (*) ()> ()

#define SET_TABLE_CONST(luaState, TABLE_NAME, KEY_NAME, CONST_VAL) 		{lua_getglobal(luaState, TABLE_NAME);\
		lua_pushinteger( luaState, CONST_VAL );\
		lua_setfield( luaState, -2, KEY_NAME );}

#define SET_G(luaState, name, intVal) {	lua_pushinteger( luaState, intVal ); lua_setglobal( luaState, -2, name );}
namespace tzw 
{
	void g_binding_game_objects()
	{
		auto luaState = static_cast<lua_State *>(ScriptPyMgr::shared()->getState());

		//CubePlayer
		BIND_START(luaState)
		BIND_BEGIN_CLASS(CubePlayer)
		BIND_FUNC(CubePlayer, getPos)
		BIND_FUNC(CubePlayer, setPos)
		BIND_FUNC(CubePlayer, getForward)
		BIND_FUNC(CubePlayer, removePartByAttach)
		BIND_END_CLASS

		//GameWorld
		BIND_START(luaState)
		BIND_BEGIN_CLASS(GameWorld)
		.addStaticFunction ("shared", &GameWorld::shared)
		BIND_FUNC(GameWorld, getCurrentState)
		BIND_FUNC(GameWorld, getPlayer)
		BIND_FUNC(GameWorld, startGame)
		BIND_END_CLASS

		//GameItem
		BIND_START(luaState)
		BIND_BEGIN_CLASS(GameItem)
		.addConstructor <void (*) ()> ()
		BIND_PROP(GameItem, m_name)
		BIND_PROP(GameItem, m_type)
		BIND_PROP(GameItem, m_desc)
		BIND_PROP(GameItem, m_class)
		BIND_END_CLASS


		//Building System
		BIND_START(luaState)
		BIND_BEGIN_CLASS(BuildingSystem)
		.addStaticFunction ("shared", &BuildingSystem::shared)
		BIND_FUNC(BuildingSystem, placeGamePart)
		BIND_FUNC(BuildingSystem, attachGamePartNormal)
		BIND_FUNC(BuildingSystem, attachGamePartToConstraint)
		BIND_FUNC(BuildingSystem, placeBearingToAttach)
		BIND_FUNC(BuildingSystem, placeSpringToAttach)
		BIND_FUNC(BuildingSystem, createPart)
		BIND_FUNC(BuildingSystem, hitTerrain)
		BIND_FUNC(BuildingSystem, rayTest)
		BIND_FUNC(BuildingSystem, placeLiftPart)
		BIND_FUNC(BuildingSystem, getLift)
		BIND_FUNC(BuildingSystem, removePartByAttach)
		BIND_FUNC(BuildingSystem, terrainForm)
		BIND_FUNC(BuildingSystem, setCurrentControlPart)
		BIND_FUNC(BuildingSystem, getCurrentControlPart)
		BIND_FUNC(BuildingSystem, getGamePartTypeInt)
		BIND_END_CLASS

		BIND_START(luaState)
		BIND_BEGIN_CLASS(MainMenu)
		.addStaticFunction ("shared", &MainMenu::shared)
		BIND_FUNC(MainMenu, isVisible)
		BIND_END_CLASS

		//GamePart and it's derived classes
		BIND_START(luaState)
		BIND_BEGIN_CLASS(GamePart)
		BIND_EMPTY_CONSTRUCT
		//BIND_FUNC(GamePart, getType)
		.endClass()
		.deriveClass<BlockPart, GamePart>("BlockPart")
		BIND_EMPTY_CONSTRUCT
		.endClass()
		.deriveClass<ControlPart, GamePart>("ControlPart")
		BIND_EMPTY_CONSTRUCT
		.endClass()
		.deriveClass<CylinderPart, GamePart>("CylinderPart")
		BIND_EMPTY_CONSTRUCT
		BIND_END_CLASS

		//Attachment
		BIND_START(luaState)
		BIND_BEGIN_CLASS(Attachment)
		BIND_EMPTY_CONSTRUCT
		BIND_PROP(Attachment, m_parent)
		BIND_PROP(Attachment, m_pos)
		BIND_PROP(Attachment, m_normal)
		BIND_PROP(Attachment, m_up)
		BIND_END_CLASS

		//BearPart
		BIND_START(luaState)
		BIND_BEGIN_CLASS(BearPart)
		BIND_EMPTY_CONSTRUCT
		BIND_PROP(BearPart, m_a)
		BIND_PROP(BearPart, m_b)
		BIND_PROP(BearPart, m_isFlipped)
		BIND_FUNC(BearPart, updateFlipped)
		BIND_END_CLASS

		//Lift Part
		BIND_START(luaState)
		BIND_BEGIN_CLASS(LiftPart)
		BIND_FUNC(LiftPart, liftUp)
		BIND_EMPTY_CONSTRUCT
		BIND_END_CLASS

		//GAME constant
		lua_newtable(luaState);
		lua_setglobal(luaState,"CPP_GAME");

		lua_getglobal(luaState, "CPP_GAME");
		lua_pushinteger( luaState, 0 );
		lua_setfield( luaState, -2, "GAME_STATE_MAIN_MENU" );

		SET_TABLE_CONST(luaState, "CPP_GAME", "GAME_STATE_MAIN_MENU", 0)
		SET_TABLE_CONST(luaState, "CPP_GAME", "GAME_STATE_RUNNING", 1)

	}
}

