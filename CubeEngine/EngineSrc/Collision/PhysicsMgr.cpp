#include "PhysicsMgr.h"
#define dSINGLE
#include "External/ode/ode.h"
namespace tzw
{
	TZW_SINGLETON_IMPL(PhysicsMgr);
	static dWorldID world;
	static dSpaceID space;
	void PhysicsMgr::start()
	{
		dInitODE2(0);
		world = dWorldCreate();
		space = dHashSpaceCreate(0);
		dWorldSetGravity(world,0,0,-9.8);
		dWorldSetCFM(world,1e-5);
		dWorldSetAutoDisableFlag(world,1);
		dWorldSetLinearDamping(world, 0.00001);
		dWorldSetAngularDamping(world, 0.005);
		dWorldSetMaxAngularSpeed(world, 200);

		dWorldSetContactMaxCorrectingVel(world,0.1);
		dWorldSetContactSurfaceLayer(world,0.001);
	}

	void PhysicsMgr::stop()
	{

	}

	void PhysicsMgr::createPlane(float x, float y, float z, float d)
	{
		dCreatePlane(space, x, y, z, d);
	}

	void PhysicsMgr::createBox(float density, float width, float height, float depth)
	{
		dMass m;
		dMassSetBox(&m,density,width, height,depth);
		auto theBox = dCreateBox(space,width, height,depth);
	}
}