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
#include "UIHelper.h"
#include "ItemMgr.h"

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


		BIND_START(luaState)
		BIND_BEGIN_CLASS(PreviewItem)
		BIND_FUNC(PreviewItem, switchAttachment)
		BIND_FUNC(PreviewItem, getCurrAttachment)
		BIND_END_CLASS
		
		//CubePlayer
		BIND_START(luaState)
		BIND_BEGIN_CLASS(CubePlayer)
		BIND_FUNC(CubePlayer, getPos)
		BIND_FUNC(CubePlayer, setPos)
		BIND_FUNC(CubePlayer, getForward)
		BIND_FUNC(CubePlayer, removePartByAttach)
		BIND_FUNC(CubePlayer, removePart)
		BIND_FUNC(CubePlayer, paint)
		BIND_FUNC(CubePlayer, setCurrSelected)
		BIND_FUNC(CubePlayer, openCurrentPartInspectMenu)
		BIND_FUNC(CubePlayer, setPreviewAngle)
		BIND_FUNC(CubePlayer, updateCrossHairTipsInfo)
		BIND_FUNC(CubePlayer, getPreviewItem)
		BIND_FUNC(CubePlayer, pressButton)
		BIND_FUNC(CubePlayer, releaseButton)
		BIND_FUNC(CubePlayer, releaseSwitch)
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
		BIND_PROP(GameItem, m_desc)
		BIND_FUNC(GameItem, getTypeInInt)
		BIND_FUNC(GameItem, isSpecialFunctionItem)
		BIND_FUNC(GameItem, getThumbNailTextureId)
		BIND_END_CLASS


		//Building System
		BIND_START(luaState)
		BIND_BEGIN_CLASS(BuildingSystem)
		.addStaticFunction ("shared", &BuildingSystem::shared)
		BIND_FUNC(BuildingSystem, placeGamePart)
		BIND_FUNC(BuildingSystem, attachGamePart)
		BIND_FUNC(BuildingSystem, attachGamePartToConstraint)
		BIND_FUNC(BuildingSystem, placeBearingToAttach)
		BIND_FUNC(BuildingSystem, placeSpringToAttach)
		BIND_FUNC(BuildingSystem, createPart)
		BIND_FUNC(BuildingSystem, hitTerrain)
		BIND_FUNC(BuildingSystem, rayTest)
		BIND_FUNC(BuildingSystem, rayTestPart)
		BIND_FUNC(BuildingSystem, rayTestPartAny)
		BIND_FUNC(BuildingSystem, flipBearing)
		BIND_FUNC(BuildingSystem, flipBearingByHit)
		BIND_FUNC(BuildingSystem, placeLiftPart)
		BIND_FUNC(BuildingSystem, getLift)
		BIND_FUNC(BuildingSystem, removePartByAttach)
		BIND_FUNC(BuildingSystem, removePart)
		BIND_FUNC(BuildingSystem, terrainForm)
		BIND_FUNC(BuildingSystem, setCurrentControlPart)
		BIND_FUNC(BuildingSystem, getCurrentControlPart)
		BIND_FUNC(BuildingSystem, getGamePartTypeInt)
		BIND_FUNC(BuildingSystem, liftStore)
		BIND_FUNC(BuildingSystem, isIsInXRayMode)
		BIND_END_CLASS
		//ItemMgr
		BIND_START(luaState)
		BIND_BEGIN_CLASS(ItemMgr)
		.addStaticFunction ("shared", &ItemMgr::shared)
		BIND_FUNC(ItemMgr, getItemByIndex)
		BIND_FUNC(ItemMgr, getItemAmount)
		BIND_FUNC(ItemMgr, getItem)
		BIND_END_CLASS
		
		BIND_START(luaState)
		BIND_BEGIN_CLASS(UIHelper)
		.addStaticFunction ("shared", &UIHelper::shared)
		BIND_FUNC(UIHelper, showFloatTips)
		BIND_END_CLASS

		BIND_START(luaState)
		BIND_BEGIN_CLASS(MainMenu)
		.addStaticFunction ("shared", &MainMenu::shared)
		BIND_FUNC(MainMenu, isVisible)
		BIND_FUNC(MainMenu, isAnyShow)
		BIND_FUNC(MainMenu, show)
		BIND_FUNC(MainMenu, hide)
		BIND_FUNC(MainMenu, setIsShowAssetEditor)
		BIND_FUNC(MainMenu, setIsShowNodeEditor)
		BIND_FUNC(MainMenu, setIsFileBroswerOpen)
		BIND_FUNC(MainMenu, isOpenAssetEditor)
		BIND_FUNC(MainMenu, setPainterShow)
		BIND_END_CLASS

		//GamePart and it's derived classes
		BIND_START(luaState)
		BIND_BEGIN_CLASS(GamePart)
		BIND_EMPTY_CONSTRUCT
		BIND_FUNC(GamePart, getPrettyAttach)
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

