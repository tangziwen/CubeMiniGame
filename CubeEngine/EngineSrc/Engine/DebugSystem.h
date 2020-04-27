#pragma once
#include "Engine/EngineDef.h"
#include "Base/Node.h"
#include <set>
#include "Math/Ray.h"
#include <map>
namespace tzw
{
class LinePrimitive;
class DebugSystem : public Singleton<DebugSystem>
{
public:
	DebugSystem();
	void handleDraw(float dt);
	void doRender(float dt);
	void drawBoundingBox(AABB aabb, Matrix44 mat);
	void drawPointCross(vec3 pointInWorld);
	void drawLine(vec3 A, vec3 B);
private:
	LinePrimitive * m_line;

};


}
