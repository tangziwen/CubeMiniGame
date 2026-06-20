#pragma once
#include "Engine/EngineDef.h"
#include "Base/Node.h"
#include "Math/AABB.h"
#include "Math/Matrix44.h"
#include <memory>
#include <vector>
namespace tzw
{
class DebugPrimitive;
class LinePrimitive;
class MaterialInstance;
class RenderQueue;
class DebugSystem : public Singleton<DebugSystem>
{
public:
	DebugSystem();
	~DebugSystem();
	void handleDraw(float dt);
	void doRender(RenderQueue * queue, float dt);
	void destroyPrimitive(DebugPrimitive* primitive);
	void setWireframeOverlayEnabled(bool enabled);
	bool isWireframeOverlayEnabled() const;
	void setWireframeOverlayDepthTestEnabled(bool enabled);
	bool isWireframeOverlayDepthTestEnabled() const;
	void setWireframeOverlayColor(vec3 color);
	vec3 getWireframeOverlayColor() const;
	RenderQueue* buildWireframeQueue(const RenderQueue* sourceQueue);

	void drawBoundingBox(AABB aabb, Matrix44 mat);
	void drawLine(vec3 A, vec3 B, vec3 color = vec3(1, 0, 0));
	void drawRay(vec3 origin, vec3 dir, float length, vec3 color = vec3(1, 0, 0));
	void drawPointCross(vec3 pointInWorld, vec3 color);
	void drawPointCross(vec3 pointInWorld, vec3 color, float size);
	void drawAABB(AABB aabb, vec3 color = vec3(1, 0, 0));
	void drawAABB(AABB aabb, Matrix44 mat, vec3 color = vec3(1, 0, 0));
	void drawBox(vec3 center, vec3 halfExtents, Matrix44 mat, vec3 color = vec3(1, 0, 0));
	void drawSphere(vec3 center, float radius, vec3 color = vec3(1, 0, 0), int segmentCount = 32);
	void drawCylinder(vec3 center, float radius, float height, vec3 color = vec3(1, 0, 0), int segmentCount = 32);
	void drawCapsule(vec3 center, float radius, float height, vec3 color = vec3(1, 0, 0), int segmentCount = 32);
	void drawCone(vec3 center, float radius, float height, vec3 color = vec3(1, 0, 0), int segmentCount = 32);
	void drawPlane(vec3 center, vec3 normal, float size, vec3 color = vec3(1, 0, 0));
	void drawGrid(vec3 center, float width, float depth, int xSegments, int zSegments, vec3 color = vec3(1, 0, 0));
	void drawFrustum(const vec3 corners[8], vec3 color = vec3(1, 0, 0));

	DebugPrimitive* createLine(vec3 A, vec3 B, vec3 color = vec3(1, 0, 0));
	DebugPrimitive* createRay(vec3 origin, vec3 dir, float length, vec3 color = vec3(1, 0, 0));
	DebugPrimitive* createPointCross(vec3 pointInWorld, vec3 color, float size);
	DebugPrimitive* createAABB(AABB aabb, vec3 color = vec3(1, 0, 0));
	DebugPrimitive* createAABB(AABB aabb, Matrix44 mat, vec3 color = vec3(1, 0, 0));
	DebugPrimitive* createBox(vec3 center, vec3 halfExtents, Matrix44 mat, vec3 color = vec3(1, 0, 0));
	DebugPrimitive* createSphere(vec3 center, float radius, vec3 color = vec3(1, 0, 0), int segmentCount = 32);
	DebugPrimitive* createCylinder(vec3 center, float radius, float height, vec3 color = vec3(1, 0, 0), int segmentCount = 32);
	DebugPrimitive* createCapsule(vec3 center, float radius, float height, vec3 color = vec3(1, 0, 0), int segmentCount = 32);
	DebugPrimitive* createCone(vec3 center, float radius, float height, vec3 color = vec3(1, 0, 0), int segmentCount = 32);
	DebugPrimitive* createPlane(vec3 center, vec3 normal, float size, vec3 color = vec3(1, 0, 0));
	DebugPrimitive* createGrid(vec3 center, float width, float depth, int xSegments, int zSegments, vec3 color = vec3(1, 0, 0));
	DebugPrimitive* createFrustum(const vec3 corners[8], vec3 color = vec3(1, 0, 0));
private:
	DebugPrimitive* createRetainedPrimitive(vec3 defaultColor);
	void removeDestroyRequestedPrimitives();
	MaterialInstance* getDebugWireframeMaterial();

	std::unique_ptr<LinePrimitive> m_immediateLine;
	std::vector<std::unique_ptr<DebugPrimitive>> m_retainedPrimitives;
	std::unique_ptr<RenderQueue> m_wireframeQueue;
	MaterialInstance* m_debugWireframeMaterial;
	bool m_wireframeOverlayEnabled;
	bool m_wireframeOverlayDepthTestEnabled;
	vec3 m_wireframeOverlayColor;

};


}
