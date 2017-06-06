#ifndef TZW_PHYSIC_MGR_H
#define TZW_PHYSIC_MGR_H
#include "../Engine/EngineDef.h"
namespace tzw
{
	class PhysicsMgr{
	public:
		TZW_SINGLETON_DECL(PhysicsMgr);
		void start();
		void stop();
		void createPlane(float x, float y, float z, float d);
		void createBox(float density, float width, float height, float depth);

	};

}
#endif // !TZW_PHYSIC_MGR_H
