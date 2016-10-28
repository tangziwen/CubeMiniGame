#include "Application/GameEntry.h"

#include "EngineSrc/Font/Font.h"

#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/2D/TableView.h"
#include "CubeGame/GameWorld.h"
#include "EngineSrc/Action/RepeatForever.h"
#include "EngineSrc/Action/RotateBy.h"
#include "EngineSrc/3D/Primitive/CubePrimitive.h"
#include "noise/noise.h"
#include "noise/noiseutils.h"
#include "EngineSrc/Math/t_Sphere.h"
#include <iostream>
#include "EngineSrc/Texture/TextureAtlas.h"
#include "EngineSrc/3D/Primitive/LinePrimitive.h"
#include "EngineSrc/3D/Primitive/LineGrid.h"
#include "EngineSrc/3D/Vegetation/Grass.h"
using namespace tzw;
GameEntry::GameEntry()
{
	m_ticks = 0;
}


void GameEntry::onStart()
{
	TextureAtlas("./Res/User/CubeGame/texture/dest/blocks.json");
//	auto model = Model::create("./Res/TestRes/duck.tzw");
//	g_GetCurrScene()->addNode(model);
//	model->setPos(vec3(0, 0, -30));
//	model->setScale(vec3(0.1, 0.1, 0.1));

//	auto gridXZ = new LineGrid(10,10);
//	gridXZ->getMaterial()->setVar("color",vec4::fromRGB(255, 0, 0,255));
//	model->addChild(gridXZ);

//	auto gridXY = new LineGrid(10,10);
//	gridXY->setRotateE(vec3(-90, 0, 0));
//	gridXY->getMaterial()->setVar("color",vec4::fromRGB(0, 255, 0,255));
//	model->addChild(gridXY);

//	auto gridYZ = new LineGrid(10,10);
//	gridYZ->setRotateE(vec3(0, 0, -90));
//	gridYZ->getMaterial()->setVar("color",vec4::fromRGB(0, 0, 255,255));
//	model->addChild(gridYZ);
//	model->runAction(new RepeatForever(new RotateBy(1.0, vec3(0, 60, 0))));
//	Engine::shared()->setClearColor(104.0/ 255.0, 180.0 / 255.0, 1.0);
//	g_GetCurrScene()->addNode(new Grass("./Res/User/CubeGame/texture/grass.png"));
//	auto camera = g_GetCurrScene()->defaultCamera();
//	camera->setPos(vec3(0,0,1));

	//Engine::shared()->setIsEnableOutLine(true);
	GameWorld::shared();
}

void GameEntry::onExit()
{

}

void GameEntry::onUpdate(float delta)
{
	m_ticks += delta;
}

