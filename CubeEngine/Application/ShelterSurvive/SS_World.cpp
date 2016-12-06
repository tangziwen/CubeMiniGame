#include "SS_World.h"
#include "3D/Primitive/CubePrimitive.h"
#include "Scene/SceneMgr.h"

namespace tzw
{
	TZW_SINGLETON_IMPL(SS_World);
	void SS_World::createWorld()
	{

	}

	static vec3 testBoxPosList[4] = {vec3(-30,7.5,-30), vec3(-30,7.5,30), vec3(30,7.5,-30), vec3(30,7.5,30)};
	void SS_World::createTest()
	{
		auto ground = new CubePrimitive(100, 100, 1.0);
		g_GetCurrScene()->addNode(ground);

		for(int i = 0; i < 4; i++)
		{
			auto testBox = new CubePrimitive(10, 10, 15);
			g_GetCurrScene()->addNode(testBox);
			testBox->setColor(vec4::fromRGB(255,0,0));
			testBox->setPos(testBoxPosList[i]);
		}
	}
}