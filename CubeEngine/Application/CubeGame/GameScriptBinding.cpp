#include "BuildingSystem.h"
#include "CubePlayer.h"
#include "CylinderPart.h"
#include "GameItem.h"
#include "GameWorld.h"
#include "GameItemMgr.h"
#include "LiftPart.h"
#include "ScriptPy/ScriptPyMgr.h"
#include "UIHelper.h"
#include "Gun/FPGun.h"

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

  BIND_BEGIN_CLASS_PY(FPGun)
	BIND_FUNC_PY(FPGun, toggleADS)
	BIND_FUNC_PY(FPGun, shoot);

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
  BIND_FUNC_PY(CubePlayer, getGun)
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
  .def("hitTerrain", static_cast<vec3 (BuildingSystem::*)(vec3, vec3, float)>(&BuildingSystem::hitTerrain), pybind11::return_value_policy::reference)
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

  .def("terrainForm", static_cast<void (BuildingSystem::*)(vec3, vec3, float, float, float)>(&BuildingSystem::terrainForm), pybind11::return_value_policy::reference)
  .def("terrainFormBox", static_cast<void (BuildingSystem::*)(vec3, vec3, float, float, vec3)>(&BuildingSystem::terrainFormBox), pybind11::return_value_policy::reference)
  .def("terrainPaint", static_cast<void (BuildingSystem::*)(vec3, vec3, float, int, float)>(&BuildingSystem::terrainPaint), pybind11::return_value_policy::reference)
  BIND_FUNC_PY(BuildingSystem, setCurrentControlPart)
  BIND_FUNC_PY(BuildingSystem, getCurrentControlPart)
  BIND_FUNC_PY(BuildingSystem, getGamePartTypeInt)
  BIND_FUNC_PY(BuildingSystem, liftStore)
  BIND_FUNC_PY(BuildingSystem, isIsInXRayMode);

  // GameItemMgr

  BIND_BEGIN_CLASS_PY(GameItemMgr)
  BIND_SINGLETON_PY(GameItemMgr)
  BIND_FUNC_PY(GameItemMgr, getItemByIndex)
  BIND_FUNC_PY(GameItemMgr, getItemAmount)
  BIND_FUNC_PY(GameItemMgr, getItem);

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
  // Keep this translation unit linked so the Game embedded Python module is registered.
}
}
