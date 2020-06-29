#include "GameScriptBinding.h"
#include "BuildingSystem.h"
#include "CubePlayer.h"
#include "CylinderPart.h"
#include "GameItem.h"
#include "GameWorld.h"
#include "ItemMgr.h"
#include "LiftPart.h"
#include "Lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "ScriptPy/ScriptPyMgr.h"
#include "UIHelper.h"

#define BIND_PROP(className, PROP) .addProperty(#PROP, &className## ::##PROP)
#define BIND_FUNC(className, FUNC) .addFunction(#FUNC, &className## ::##FUNC)
#define BIND_START(state) luabridge::getGlobalNamespace(state)
#define BIND_BEGIN_CLASS(className) .beginClass<className>(#className)
#define BIND_END_CLASS .endClass();
#define BIND_EMPTY_CONSTRUCT .addConstructor<void (*)()>()

#define SET_TABLE_CONST(luaState, TABLE_NAME, KEY_NAME, CONST_VAL)             \
  {                                                                            \
    lua_getglobal(luaState, TABLE_NAME);                                       \
    lua_pushinteger(luaState, CONST_VAL);                                      \
    lua_setfield(luaState, -2, KEY_NAME);                                      \
  }

#define SET_G(luaState, name, intVal)                                          \
  {                                                                            \
    lua_pushinteger(luaState, intVal);                                         \
    lua_setglobal(luaState, -2, name);                                         \
  }

// #define BIND_BEGIN_CLASS_PY(className)                                         \
//   pybind11::class_<className>(m, #className)
// #define BIND_PROP_PY(className, FUNC)                                          \
//   .def_readwrite(#FUNC, &className## ::##FUNC)
// #define BIND_FUNC_PY(className, FUNC)                                          \
//   .def(#FUNC, &className## ::##FUNC, pybind11::return_value_policy::reference)
// #define BIND_STATIC_FUNC_PY(className, FUNC)                                   \
//   .def_static(#FUNC, &className## ::##FUNC)
// #define BIND_SINGLETON_PY(className)                                           \
//   .def_static(                                                                 \
//     "shared", &className## ::shared, pybind11::return_value_policy::reference)
//
// #define BIND_MODULE_FUNC_PY(className, FUNC) m.def(#FUNC, &className## ::##FUNC)
// #define BIND_MODULE_CUSTOM_FUNC_PY(FUNCStr, FUNCPtr) m.def(FUNCStr, FUNCPtr)
// #define BIND_EMPTY_CONSTRUCT_PY .def(py::init<>())
#include "ScriptPy/ScriptBindMacro.h"
#include <pybind11/embed.h>
namespace py = pybind11;

namespace tzw {
PYBIND11_EMBEDDED_MODULE(Game, m)
{
  m.doc() = "Game"; // optional module docstring

  BIND_BEGIN_CLASS_PY(PreviewItem)
    .def(py::init<>()) BIND_FUNC_PY(PreviewItem, switchAttachment)
      BIND_FUNC_PY(PreviewItem, getCurrAttachment);

  // CubePlayer
  BIND_BEGIN_CLASS_PY(CubePlayer)
  BIND_FUNC_PY(CubePlayer, getPos)
  BIND_FUNC_PY(CubePlayer, setPos)
  BIND_FUNC_PY(CubePlayer, getForward)
  BIND_FUNC_PY(CubePlayer, removePartByAttach)
  BIND_FUNC_PY(CubePlayer, removePart)
  BIND_FUNC_PY(CubePlayer, paint)
  BIND_FUNC_PY(CubePlayer, setCurrSelected)
  BIND_FUNC_PY(CubePlayer, openCurrentPartInspectMenu)
  BIND_FUNC_PY(CubePlayer, setPreviewAngle)
  BIND_FUNC_PY(CubePlayer, updateCrossHairTipsInfo)
  BIND_FUNC_PY(CubePlayer, getPreviewItem)
  BIND_FUNC_PY(CubePlayer, pressButton)
  BIND_FUNC_PY(CubePlayer, releaseButton)
  BIND_FUNC_PY(CubePlayer, releaseSwitch);

  // GameWorld
  BIND_BEGIN_CLASS_PY(GameWorld)
  BIND_SINGLETON_PY(GameWorld)
  BIND_FUNC_PY(GameWorld, getCurrentState)
  BIND_FUNC_PY(GameWorld, getPlayer)
  BIND_FUNC_PY(GameWorld, startGame);

  BIND_BEGIN_CLASS_PY(PhysicsInfo)
  BIND_PROP_PY(PhysicsInfo, mass)
  BIND_PROP_PY(PhysicsInfo, size);

  // GameItem

  BIND_BEGIN_CLASS_PY(GameItem)
  BIND_PROP_PY(GameItem, m_name)
  BIND_PROP_PY(GameItem, m_desc)
  BIND_PROP_PY(GameItem, m_description)
  BIND_PROP_PY(GameItem, m_physicsInfo)
  BIND_FUNC_PY(GameItem, getTypeInInt)
  BIND_FUNC_PY(GameItem, isSpecialFunctionItem)
  BIND_FUNC_PY(GameItem, getThumbNailTextureId);



  // Building System

  BIND_BEGIN_CLASS_PY(BuildingSystem)
  BIND_SINGLETON_PY(BuildingSystem)
  BIND_FUNC_PY(BuildingSystem, placeGamePartStatic)
  BIND_FUNC_PY(BuildingSystem, attachGamePart)
  BIND_FUNC_PY(BuildingSystem, attachGamePartToConstraint)
  BIND_FUNC_PY(BuildingSystem, placeBearingToAttach)
  BIND_FUNC_PY(BuildingSystem, placeSpringToAttach)
  BIND_FUNC_PY(BuildingSystem, createPart)
  BIND_FUNC_PY(BuildingSystem, hitTerrain)
  BIND_FUNC_PY(BuildingSystem, rayTest)
  BIND_FUNC_PY(BuildingSystem, rayTestPart)
  BIND_FUNC_PY(BuildingSystem, rayTestPartAny)
  BIND_FUNC_PY(BuildingSystem, flipBearing)
  BIND_FUNC_PY(BuildingSystem, flipBearingByHit)
  BIND_FUNC_PY(BuildingSystem, placeLiftPart)
  BIND_FUNC_PY(BuildingSystem, getLift)
  BIND_FUNC_PY(BuildingSystem, removePartByAttach)
  BIND_FUNC_PY(BuildingSystem, removePart)
  BIND_FUNC_PY(BuildingSystem, removeLiftPart)
	
  BIND_FUNC_PY(BuildingSystem, terrainForm)
  BIND_FUNC_PY(BuildingSystem, setCurrentControlPart)
  BIND_FUNC_PY(BuildingSystem, getCurrentControlPart)
  BIND_FUNC_PY(BuildingSystem, getGamePartTypeInt)
  BIND_FUNC_PY(BuildingSystem, liftStore)
  BIND_FUNC_PY(BuildingSystem, isIsInXRayMode);

  // ItemMgr

  BIND_BEGIN_CLASS_PY(ItemMgr)
  BIND_SINGLETON_PY(ItemMgr)
  BIND_FUNC_PY(ItemMgr, getItemByIndex)
  BIND_FUNC_PY(ItemMgr, getItemAmount)
  BIND_FUNC_PY(ItemMgr, getItem);

  BIND_BEGIN_CLASS_PY(UIHelper)
  BIND_SINGLETON_PY(UIHelper)
  BIND_FUNC_PY(UIHelper, showFloatTips);

  BIND_BEGIN_CLASS_PY(GameUISystem)
  BIND_SINGLETON_PY(GameUISystem)
  BIND_FUNC_PY(GameUISystem, isVisible)
  BIND_FUNC_PY(GameUISystem, isAnyShow)
  BIND_FUNC_PY(GameUISystem, show)
  BIND_FUNC_PY(GameUISystem, hide)
  BIND_FUNC_PY(GameUISystem, setIsShowAssetEditor)
  BIND_FUNC_PY(GameUISystem, setIsShowNodeEditor)
  BIND_FUNC_PY(GameUISystem, setIsFileBroswerOpen)
  BIND_FUNC_PY(GameUISystem, isOpenAssetEditor)
  BIND_FUNC_PY(GameUISystem, setPainterShow)
  BIND_FUNC_PY(GameUISystem, openNodeEditor);
	

  // GamePart and it's derived classes

  BIND_BEGIN_CLASS_PY(GamePart)
  BIND_EMPTY_CONSTRUCT_PY
  BIND_FUNC_PY(GamePart, getVehicle)
  BIND_FUNC_PY(GamePart, getPrettyAttach);

  pybind11::class_<BlockPart, GamePart>(m, "BlockPart") BIND_EMPTY_CONSTRUCT_PY;

  pybind11::class_<ControlPart, GamePart>(m, "ControlPart")
    BIND_EMPTY_CONSTRUCT_PY;

  pybind11::class_<CylinderPart, GamePart>(m, "CylinderPart")
    BIND_EMPTY_CONSTRUCT_PY;

  // Attachment
  BIND_BEGIN_CLASS_PY(Attachment)
  BIND_EMPTY_CONSTRUCT_PY
  BIND_PROP_PY(Attachment, m_parent)
  BIND_PROP_PY(Attachment, m_pos)
  BIND_PROP_PY(Attachment, m_normal) BIND_PROP_PY(Attachment, m_up);

  // BearPart

  // BIND_BEGIN_CLASS_PY(BearPart)
  pybind11::class_<BearPart, GamePart>(m, "BearPart")
  BIND_PROP_PY(BearPart, m_a) BIND_PROP_PY(BearPart, m_b)
  BIND_PROP_PY(BearPart, m_isFlipped) BIND_FUNC_PY(BearPart, updateFlipped);

  // Lift Part

  // BIND_BEGIN_CLASS_PY(LiftPart)
  pybind11::class_<LiftPart, GamePart>(m, "LiftPart")
  BIND_FUNC_PY(LiftPart, liftUp)
	BIND_PROP_PY(LiftPart, m_effectedIslandGroup)
  BIND_EMPTY_CONSTRUCT_PY;

	 //Vehicle
	BIND_BEGIN_CLASS_PY(Vehicle)
	BIND_EMPTY_CONSTRUCT_PY;

  m.attr("GAME_STATE_MAIN_MENU") = 0;
  m.attr("GAME_STATE_RUNNING") = 1;
};

void
g_binding_game_objects()
{
  auto luaState = static_cast<lua_State*>(ScriptPyMgr::shared()->getState());

  BIND_START(luaState)
  BIND_BEGIN_CLASS(PreviewItem)
  BIND_FUNC(PreviewItem, switchAttachment)
  BIND_FUNC(PreviewItem, getCurrAttachment)
  BIND_END_CLASS

  // CubePlayer
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

  // GameWorld
  BIND_START(luaState)
  BIND_BEGIN_CLASS(GameWorld)
    .addStaticFunction("shared", &GameWorld::shared)
      BIND_FUNC(GameWorld, getCurrentState) BIND_FUNC(GameWorld, getPlayer)
        BIND_FUNC(GameWorld, startGame) BIND_END_CLASS

          BIND_START(luaState) BIND_BEGIN_CLASS(PhysicsInfo)
            BIND_PROP(PhysicsInfo, mass) BIND_PROP(PhysicsInfo, size)
              BIND_END_CLASS

                // GameItem
                BIND_START(luaState) BIND_BEGIN_CLASS(GameItem)
                  BIND_PROP(GameItem, m_name) BIND_PROP(GameItem, m_desc)
                    BIND_PROP(GameItem, m_description)
                      BIND_PROP(GameItem, m_physicsInfo)
                        BIND_FUNC(GameItem, getTypeInInt)
                          BIND_FUNC(GameItem, isSpecialFunctionItem)
                            BIND_FUNC(GameItem, getThumbNailTextureId)
                              BIND_END_CLASS

                                // Building System
                                BIND_START(luaState)
                                  BIND_BEGIN_CLASS(BuildingSystem)
    .addStaticFunction("shared", &BuildingSystem::shared)
      BIND_FUNC(BuildingSystem, placeGamePartStatic) BIND_FUNC(BuildingSystem,
                                                               attachGamePart)
        BIND_FUNC(BuildingSystem, attachGamePartToConstraint)
          BIND_FUNC(BuildingSystem, placeBearingToAttach)
            BIND_FUNC(BuildingSystem, placeSpringToAttach) BIND_FUNC(
              BuildingSystem, createPart) BIND_FUNC(BuildingSystem, hitTerrain)
              BIND_FUNC(BuildingSystem, rayTest) BIND_FUNC(BuildingSystem,
                                                           rayTestPart)
                BIND_FUNC(BuildingSystem,
                          rayTestPartAny) BIND_FUNC(BuildingSystem, flipBearing)
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
                                          // ItemMgr
                                          BIND_START(luaState)
                                            BIND_BEGIN_CLASS(ItemMgr)
    .addStaticFunction("shared", &ItemMgr::shared)
      BIND_FUNC(ItemMgr, getItemByIndex) BIND_FUNC(ItemMgr, getItemAmount)
        BIND_FUNC(ItemMgr, getItem) BIND_END_CLASS

          BIND_START(luaState) BIND_BEGIN_CLASS(UIHelper)
    .addStaticFunction("shared", &UIHelper::shared)
      BIND_FUNC(UIHelper, showFloatTips) BIND_END_CLASS

        BIND_START(luaState) BIND_BEGIN_CLASS(GameUISystem)
    .addStaticFunction("shared", &GameUISystem::shared)
      BIND_FUNC(GameUISystem, isVisible) BIND_FUNC(GameUISystem, isAnyShow)
        BIND_FUNC(GameUISystem, show) BIND_FUNC(GameUISystem, hide)
          BIND_FUNC(GameUISystem, setIsShowAssetEditor)
            BIND_FUNC(GameUISystem, setIsShowNodeEditor)
              BIND_FUNC(GameUISystem, setIsFileBroswerOpen)
                BIND_FUNC(GameUISystem, isOpenAssetEditor)
                  BIND_FUNC(GameUISystem, setPainterShow) BIND_END_CLASS

                    // GamePart and it's derived classes
                    BIND_START(luaState) BIND_BEGIN_CLASS(GamePart)
                      BIND_EMPTY_CONSTRUCT BIND_FUNC(GamePart, getPrettyAttach)
    .endClass()
    .deriveClass<BlockPart, GamePart>("BlockPart")
      BIND_EMPTY_CONSTRUCT.endClass()
    .deriveClass<ControlPart, GamePart>("ControlPart")
      BIND_EMPTY_CONSTRUCT.endClass()
    .deriveClass<CylinderPart, GamePart>("CylinderPart")
      BIND_EMPTY_CONSTRUCT BIND_END_CLASS

        // Attachment
        BIND_START(luaState) BIND_BEGIN_CLASS(Attachment)
          BIND_EMPTY_CONSTRUCT BIND_PROP(Attachment, m_parent)
            BIND_PROP(Attachment, m_pos) BIND_PROP(Attachment, m_normal)
              BIND_PROP(Attachment, m_up) BIND_END_CLASS

                // BearPart
                BIND_START(luaState) BIND_BEGIN_CLASS(BearPart)
                  BIND_PROP(BearPart, m_a) BIND_PROP(BearPart, m_b)
                    BIND_PROP(BearPart, m_isFlipped)
                      BIND_FUNC(BearPart, updateFlipped) BIND_END_CLASS

                        // Lift Part
                        BIND_START(luaState) BIND_BEGIN_CLASS(LiftPart)
                          BIND_FUNC(LiftPart, liftUp)
                            BIND_EMPTY_CONSTRUCT BIND_END_CLASS

                              // GAME constant
                              lua_newtable(luaState);
  lua_setglobal(luaState, "CPP_GAME");

  lua_getglobal(luaState, "CPP_GAME");
  lua_pushinteger(luaState, 0);
  lua_setfield(luaState, -2, "GAME_STATE_MAIN_MENU");

  SET_TABLE_CONST(luaState, "CPP_GAME", "GAME_STATE_MAIN_MENU", 0)
  SET_TABLE_CONST(luaState, "CPP_GAME", "GAME_STATE_RUNNING", 1)
}
}
