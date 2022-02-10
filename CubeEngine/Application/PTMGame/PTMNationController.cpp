#include "PTMNationController.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMTile.h"
#include "PTMNation.h"
#include "2D/LabelNew.h"
namespace tzw
{
	PTMNationController::PTMNationController():m_nation(nullptr)
	{
	}
	void PTMNationController::control(PTMNation* nation)
	{
		m_nation = nation;
	}
}