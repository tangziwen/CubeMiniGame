#include "Application/GameEntry.h"

#include "EngineSrc/Font/Font.h"

#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/2D/TableView.h"
#include "CubeGame/GameWorld.h"
#include "EngineSrc/Action/RepeatForever.h"
#include "EngineSrc/Action/RotateBy.h"
#include "EngineSrc/3D/Primitive/CubePrimitive.h"
#include "EngineSrc/3D/Terrain/TerrainChunk.h"
#include "EngineSrc/3D/Terrain/VoxelChunk.h"
#include "noise/noise.h"
#include "noise/noiseutils.h"
using namespace tzw;
GameEntry::GameEntry()
{
    m_ticks = 0;
}

#define MINX -4.0
#define MAXX 5.0
#define MINY -8.0
#define MAXY 8.0
#define MINZ -8.0
#define MAXZ 8.0

float Potential(vec3 p)
{
    auto dp1 = vec3( 0.0, -2.0,  0.0)-p;
    auto dp2 = vec3( 0.0,  2.0,  0.0)-p;
    auto dp3 = vec3( 2.0,  2.0,  0.0)-p;
    auto dp4 = vec3( 0.0,  0.0,  4.0)-p;
    auto dp5 = vec3(-0.5,  3.1, -1.0)-p;
    auto dp6 = vec3( 0.0,  0.0, -4.0)-p;
    return 1/dp1.length() + 1/dp2.length() + 1/dp3.length() + 1/dp4.length() + 1/dp5.length() +
        1/dp6.length();

}
void GameEntry::onStart()
{
//    FPSCamera * camera = FPSCamera::create(SceneMgr::shared()->currentScene()->defaultCamera());
//    SceneMgr::shared()->currentScene()->setDefaultCamera(camera);
//    SceneMgr::shared()->currentScene()->addNode(camera);
//    camera->setIsEnableGravity(false);
//    camera->setPos(vec3(0,15,15));
//    camera->lookAt(vec3(0,0,0));
//    auto voxel = new VoxelChunk();
//    int nX = 30, nY = 30, nZ = 30;
//    auto mcPoints = new vec4[(nX+1)*(nY+1)*(nZ+1)];
//    auto theSize = (nX+1)*(nY+1)*(nZ+1);
//    vec3 stepSize((MAXX-MINX)/nX, (MAXY-MINY)/nY, (MAXZ-MINZ)/nZ);
//    for(int i=0; i < nX+1; i++)
//        for(int j=0; j < nY+1; j++)
//            for(int k=0; k < nZ+1; k++) {
//                vec4 vert(MINX+i*stepSize.x, MINY+j*stepSize.y, MINZ+k*stepSize.z, 0);
//                vert.w = Potential(vert.toVec3());
//                mcPoints[i*(nY+1)*(nZ+1) + j*(nZ+1) + k] = vert;
//            }
//    voxel->init(nX, nY, nZ, mcPoints, 1.8);
//    SceneMgr::shared()->currentScene()->addNode(voxel);
//    Engine::shared()->setIsEnableOutLine(true);
        GameWorld::shared();
}

void GameEntry::onExit()
{

}

void GameEntry::onUpdate(float delta)
{
    m_ticks += delta;

    //    if (m_ticks > 1.0) //每2秒过一天
    //    {
    //        tzwS::GameSystem::shared()->update();
    //        m_ticks = 0;
    //    }

}

