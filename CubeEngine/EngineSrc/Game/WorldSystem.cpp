#include "WorldSystem.h"
#include "EngineSrc/3D/Primitive/CubePrimitive.h"
#include "EngineSrc/Scene/SceneMgr.h"
#include "EngineSrc/3D/Model/Model.h"

using namespace tzw;
namespace TDS {


TZW_SINGLETON_IMPL(WorldSystem)

WorldSystem::WorldSystem()
{
	m_camera = new GameCamera();
}

void WorldSystem::generate()
{
	auto playerModel = Model::create("./Res/chr_man.tzw");
	g_GetCurrScene()->addNode(playerModel);
	playerModel->setScale(vec3(0.2, 0.2,0.2));
	playerModel->setPos(0,0, -10);
	auto camera = g_GetCurrScene()->defaultCamera();
	camera->setPos(0, 25, 0);
	camera->lookAt(playerModel->getPos(), vec3(0, 1.0, 0));
	for(int i = 0; i < 25; i++)
	{
		for(int k = 0; k < 25; k++)
		{
			auto cube = new CubePrimitive(1, 1, 1);
			g_GetCurrScene()->addNode(cube);
			cube->setPos(i, 0, -1 * k);
		}
	}
}

} // namespace TDS
