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
#include "FMod/fmod.hpp"
#include "AudioSystem/AudioSystem.h"

using namespace tzw;
using namespace std;

GameEntry::GameEntry()
{
	m_ticks = 0;
}

void GameEntry::onStart()
{ 

#if CURRGAME == GAME_MODE_PLAYGROUND
	GameWorld::shared();
#endif
}

void GameEntry::onExit()
{

}

void GameEntry::onUpdate(float delta)
{
	
}