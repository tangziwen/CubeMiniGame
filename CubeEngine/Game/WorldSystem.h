#ifndef TDS_WORLDSYSTEM_H
#define TDS_WORLDSYSTEM_H

#include "EngineSrc/Engine/EngineDef.h"
#include "GameCamera.h"
namespace TDS {

class WorldSystem
{
public:
	TZW_SINGLETON_DECL(WorldSystem)
	WorldSystem();
	void generate();
private:
	GameCamera * m_camera;
};

} // namespace TDS

#endif // TDS_WORLDSYSTEM_H
