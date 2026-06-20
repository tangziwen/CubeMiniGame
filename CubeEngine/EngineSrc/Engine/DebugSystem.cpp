#include "DebugSystem.h"
#include "DebugPrimitive.h"
#include "Scene/SceneMgr.h"
#include "2D/LabelNew.h"
#include "3D/Primitive/LinePrimitive.h"
#include "Rendering/RenderQueues.h"
#include "Technique/Material.h"
#include <algorithm>
#include <cmath>

namespace tzw
{
namespace
{
constexpr float DebugPi = 3.14159265358979323846f;

int clampSegmentCount(int segmentCount)
{
	return std::max(8, std::min(segmentCount, 128));
}

vec3 safeNormalize(vec3 value, vec3 fallback)
{
	if(value.squaredLength() <= 0.000001f)
	{
		return fallback;
	}
	value.normalize();
	return value;
}

void appendBoxCorners(LinePrimitive* line, const vec3 corners[8], vec3 color)
{
	line->append(corners[0], corners[1], color);
	line->append(corners[0], corners[4], color);
	line->append(corners[0], corners[3], color);
	line->append(corners[3], corners[2], color);
	line->append(corners[3], corners[7], color);
	line->append(corners[6], corners[2], color);
	line->append(corners[6], corners[7], color);
	line->append(corners[6], corners[5], color);
	line->append(corners[5], corners[4], color);
	line->append(corners[5], corners[1], color);
	line->append(corners[1], corners[2], color);
	line->append(corners[4], corners[7], color);
}

void buildAABB(LinePrimitive* line, AABB aabb, Matrix44 mat, vec3 color)
{
	vec3 minV = aabb.min();
	vec3 maxV = aabb.max();
	vec3 corners[8];
	corners[0] = mat.transformVec3(minV);
	corners[1] = mat.transformVec3(vec3(maxV.x, minV.y, minV.z));
	corners[2] = mat.transformVec3(vec3(maxV.x, maxV.y, minV.z));
	corners[3] = mat.transformVec3(vec3(minV.x, maxV.y, minV.z));
	corners[4] = mat.transformVec3(vec3(minV.x, minV.y, maxV.z));
	corners[5] = mat.transformVec3(vec3(maxV.x, minV.y, maxV.z));
	corners[6] = mat.transformVec3(maxV);
	corners[7] = mat.transformVec3(vec3(minV.x, maxV.y, maxV.z));
	appendBoxCorners(line, corners, color);
}

void buildPointCross(LinePrimitive* line, vec3 pointInWorld, vec3 color, float size)
{
	line->append(pointInWorld + vec3(-size, 0, 0), pointInWorld + vec3(size, 0, 0), color);
	line->append(pointInWorld + vec3(0, -size, 0), pointInWorld + vec3(0, size, 0), color);
	line->append(pointInWorld + vec3(0, 0, -size), pointInWorld + vec3(0, 0, size), color);
}

void buildLine(LinePrimitive* line, vec3 A, vec3 B, vec3 color)
{
	line->append(A, B, color);
}

void buildRay(LinePrimitive* line, vec3 origin, vec3 dir, float length, vec3 color)
{
	vec3 normalizedDir = safeNormalize(dir, vec3(0, 0, 1));
	line->append(origin, origin + normalizedDir * length, color);
}

void buildBox(LinePrimitive* line, vec3 center, vec3 halfExtents, Matrix44 mat, vec3 color)
{
	vec3 corners[8];
	corners[0] = mat.transformVec3(center + vec3(-halfExtents.x, -halfExtents.y, -halfExtents.z));
	corners[1] = mat.transformVec3(center + vec3(halfExtents.x, -halfExtents.y, -halfExtents.z));
	corners[2] = mat.transformVec3(center + vec3(halfExtents.x, halfExtents.y, -halfExtents.z));
	corners[3] = mat.transformVec3(center + vec3(-halfExtents.x, halfExtents.y, -halfExtents.z));
	corners[4] = mat.transformVec3(center + vec3(-halfExtents.x, -halfExtents.y, halfExtents.z));
	corners[5] = mat.transformVec3(center + vec3(halfExtents.x, -halfExtents.y, halfExtents.z));
	corners[6] = mat.transformVec3(center + vec3(halfExtents.x, halfExtents.y, halfExtents.z));
	corners[7] = mat.transformVec3(center + vec3(-halfExtents.x, halfExtents.y, halfExtents.z));
	appendBoxCorners(line, corners, color);
}

void buildCircle(LinePrimitive* line, vec3 center, vec3 axisA, vec3 axisB, vec3 color, int segmentCount)
{
	int count = clampSegmentCount(segmentCount);
	for(int i = 0; i < count; i++)
	{
		float angleA = (static_cast<float>(i) / static_cast<float>(count)) * DebugPi * 2.0f;
		float angleB = (static_cast<float>(i + 1) / static_cast<float>(count)) * DebugPi * 2.0f;
		vec3 pointA = center + axisA * std::cos(angleA) + axisB * std::sin(angleA);
		vec3 pointB = center + axisA * std::cos(angleB) + axisB * std::sin(angleB);
		line->append(pointA, pointB, color);
	}
}

void buildArc(LinePrimitive* line, vec3 center, vec3 axisA, vec3 axisB, float startAngle, float endAngle, vec3 color, int segmentCount)
{
	int count = clampSegmentCount(segmentCount) / 2;
	for(int i = 0; i < count; i++)
	{
		float tA = static_cast<float>(i) / static_cast<float>(count);
		float tB = static_cast<float>(i + 1) / static_cast<float>(count);
		float angleA = startAngle + (endAngle - startAngle) * tA;
		float angleB = startAngle + (endAngle - startAngle) * tB;
		vec3 pointA = center + axisA * std::cos(angleA) + axisB * std::sin(angleA);
		vec3 pointB = center + axisA * std::cos(angleB) + axisB * std::sin(angleB);
		line->append(pointA, pointB, color);
	}
}

void buildSphere(LinePrimitive* line, vec3 center, float radius, vec3 color, int segmentCount)
{
	buildCircle(line, center, vec3(radius, 0, 0), vec3(0, radius, 0), color, segmentCount);
	buildCircle(line, center, vec3(0, radius, 0), vec3(0, 0, radius), color, segmentCount);
	buildCircle(line, center, vec3(radius, 0, 0), vec3(0, 0, radius), color, segmentCount);
}

void buildCylinder(LinePrimitive* line, vec3 center, float radius, float height, vec3 color, int segmentCount)
{
	float halfHeight = height * 0.5f;
	vec3 top = center + vec3(0, halfHeight, 0);
	vec3 bottom = center + vec3(0, -halfHeight, 0);
	buildCircle(line, top, vec3(radius, 0, 0), vec3(0, 0, radius), color, segmentCount);
	buildCircle(line, bottom, vec3(radius, 0, 0), vec3(0, 0, radius), color, segmentCount);
	line->append(top + vec3(radius, 0, 0), bottom + vec3(radius, 0, 0), color);
	line->append(top + vec3(-radius, 0, 0), bottom + vec3(-radius, 0, 0), color);
	line->append(top + vec3(0, 0, radius), bottom + vec3(0, 0, radius), color);
	line->append(top + vec3(0, 0, -radius), bottom + vec3(0, 0, -radius), color);
}

void buildCapsule(LinePrimitive* line, vec3 center, float radius, float height, vec3 color, int segmentCount)
{
	float cylinderHeight = std::max(0.0f, height - radius * 2.0f);
	float halfCylinderHeight = cylinderHeight * 0.5f;
	vec3 top = center + vec3(0, halfCylinderHeight, 0);
	vec3 bottom = center + vec3(0, -halfCylinderHeight, 0);
	buildCylinder(line, center, radius, cylinderHeight, color, segmentCount);
	buildArc(line, top, vec3(radius, 0, 0), vec3(0, radius, 0), 0.0f, DebugPi, color, segmentCount);
	buildArc(line, top, vec3(0, 0, radius), vec3(0, radius, 0), 0.0f, DebugPi, color, segmentCount);
	buildArc(line, bottom, vec3(radius, 0, 0), vec3(0, radius, 0), DebugPi, DebugPi * 2.0f, color, segmentCount);
	buildArc(line, bottom, vec3(0, 0, radius), vec3(0, radius, 0), DebugPi, DebugPi * 2.0f, color, segmentCount);
}

void buildCone(LinePrimitive* line, vec3 center, float radius, float height, vec3 color, int segmentCount)
{
	float halfHeight = height * 0.5f;
	vec3 base = center + vec3(0, -halfHeight, 0);
	vec3 tip = center + vec3(0, halfHeight, 0);
	buildCircle(line, base, vec3(radius, 0, 0), vec3(0, 0, radius), color, segmentCount);
	line->append(tip, base + vec3(radius, 0, 0), color);
	line->append(tip, base + vec3(-radius, 0, 0), color);
	line->append(tip, base + vec3(0, 0, radius), color);
	line->append(tip, base + vec3(0, 0, -radius), color);
}

void buildPlane(LinePrimitive* line, vec3 center, vec3 normal, float size, vec3 color)
{
	vec3 n = safeNormalize(normal, vec3(0, 1, 0));
	vec3 tangent = safeNormalize(vec3::CrossProduct(n, vec3(0, 1, 0)), vec3(1, 0, 0));
	vec3 bitangent = safeNormalize(vec3::CrossProduct(n, tangent), vec3(0, 0, 1));
	float halfSize = size * 0.5f;
	vec3 corners[4] =
	{
		center + tangent * -halfSize + bitangent * -halfSize,
		center + tangent * halfSize + bitangent * -halfSize,
		center + tangent * halfSize + bitangent * halfSize,
		center + tangent * -halfSize + bitangent * halfSize
	};
	line->append(corners[0], corners[1], color);
	line->append(corners[1], corners[2], color);
	line->append(corners[2], corners[3], color);
	line->append(corners[3], corners[0], color);
	line->append(center, center + n * (size * 0.25f), color);
}

void buildGrid(LinePrimitive* line, vec3 center, float width, float depth, int xSegments, int zSegments, vec3 color)
{
	int safeXSegments = std::max(1, std::min(xSegments, 128));
	int safeZSegments = std::max(1, std::min(zSegments, 128));
	float halfWidth = width * 0.5f;
	float halfDepth = depth * 0.5f;
	for(int i = 0; i <= safeXSegments; i++)
	{
		float x = -halfWidth + width * static_cast<float>(i) / static_cast<float>(safeXSegments);
		line->append(center + vec3(x, 0, -halfDepth), center + vec3(x, 0, halfDepth), color);
	}
	for(int i = 0; i <= safeZSegments; i++)
	{
		float z = -halfDepth + depth * static_cast<float>(i) / static_cast<float>(safeZSegments);
		line->append(center + vec3(-halfWidth, 0, z), center + vec3(halfWidth, 0, z), color);
	}
}

void buildFrustum(LinePrimitive* line, const vec3 corners[8], vec3 color)
{
	line->append(corners[0], corners[1], color);
	line->append(corners[1], corners[2], color);
	line->append(corners[2], corners[3], color);
	line->append(corners[3], corners[0], color);
	line->append(corners[4], corners[5], color);
	line->append(corners[5], corners[6], color);
	line->append(corners[6], corners[7], color);
	line->append(corners[7], corners[4], color);
	line->append(corners[0], corners[4], color);
	line->append(corners[1], corners[5], color);
	line->append(corners[2], corners[6], color);
	line->append(corners[3], corners[7], color);
}

} // namespace

DebugSystem::DebugSystem()
	:m_immediateLine(new LinePrimitive()),
	m_wireframeQueue(new RenderQueue()),
	m_debugWireframeMaterial(nullptr),
	m_wireframeOverlayEnabled(true),
	m_wireframeOverlayDepthTestEnabled(true),
	m_wireframeOverlayColor(1, 1, 1)
{
}

DebugSystem::~DebugSystem()
{
	delete m_debugWireframeMaterial;
	m_debugWireframeMaterial = nullptr;
}

void DebugSystem::handleDraw(float dt)
{
	m_immediateLine->clear();
	removeDestroyRequestedPrimitives();
}

void DebugSystem::doRender(RenderQueue * queue,float dt)
{
	if(m_immediateLine->getLineCount() > 0)
	{
		m_immediateLine->initBuffer();
		m_immediateLine->submitDrawCmd(RenderFlag::RenderStage::COMMON, queue, 0);
	}
	for(auto& primitive : m_retainedPrimitives)
	{
		primitive->submitDrawCmd(queue, 0);
	}
}

void DebugSystem::destroyPrimitive(DebugPrimitive* primitive)
{
	if(!primitive)
	{
		return;
	}
	primitive->requestDestroy();
	removeDestroyRequestedPrimitives();
}

void DebugSystem::setWireframeOverlayEnabled(bool enabled)
{
	m_wireframeOverlayEnabled = enabled;
}

bool DebugSystem::isWireframeOverlayEnabled() const
{
	return m_wireframeOverlayEnabled;
}

void DebugSystem::setWireframeOverlayDepthTestEnabled(bool enabled)
{
	m_wireframeOverlayDepthTestEnabled = enabled;
	if(m_debugWireframeMaterial)
	{
		m_debugWireframeMaterial->setIsDepthTestEnable(m_wireframeOverlayDepthTestEnabled);
	}
}

bool DebugSystem::isWireframeOverlayDepthTestEnabled() const
{
	return m_wireframeOverlayDepthTestEnabled;
}

void DebugSystem::setWireframeOverlayColor(vec3 color)
{
	m_wireframeOverlayColor = color;
	if(m_debugWireframeMaterial)
	{
		m_debugWireframeMaterial->setVar("TU_color", vec4(m_wireframeOverlayColor, 1.0f));
	}
}

vec3 DebugSystem::getWireframeOverlayColor() const
{
	return m_wireframeOverlayColor;
}

RenderQueue* DebugSystem::buildWireframeQueue(const RenderQueue* sourceQueue)
{
	m_wireframeQueue->clearCommands();
	if(!sourceQueue)
	{
		return m_wireframeQueue.get();
	}

	Material* wireframeMaterial = getDebugWireframeMaterial();
	for(auto& command : sourceQueue->getList())
	{
		if(!command.hasRenderStage(RenderFlag::RenderStage::DEBUG_LAYER))
		{
			continue;
		}
		RenderCommand overlayCommand = command;
		overlayCommand.setMat(wireframeMaterial);
		overlayCommand.setRenderStageMask(static_cast<uint32_t>(RenderFlag::RenderStage::DEBUG_LAYER));
		m_wireframeQueue->addRenderCommand(overlayCommand, 0);
	}
	return m_wireframeQueue.get();
}

void DebugSystem::drawBoundingBox(AABB aabb, Matrix44 mat)
{
	drawAABB(aabb, mat, vec3(1, 1, 1));
}

void DebugSystem::drawPointCross(vec3 pointInWorld, vec3 color)
{
	drawPointCross(pointInWorld, color, 0.1f);
}

void DebugSystem::drawPointCross(vec3 pointInWorld, vec3 color, float size)
{
	buildPointCross(m_immediateLine.get(), pointInWorld, color, size);
}

void DebugSystem::drawLine(vec3 A, vec3 B, vec3 color)
{
	buildLine(m_immediateLine.get(), A, B, color);
}

void DebugSystem::drawRay(vec3 origin, vec3 dir, float length, vec3 color)
{
	buildRay(m_immediateLine.get(), origin, dir, length, color);
}

void DebugSystem::drawAABB(AABB aabb, vec3 color)
{
	Matrix44 identity;
	buildAABB(m_immediateLine.get(), aabb, identity, color);
}

void DebugSystem::drawAABB(AABB aabb, Matrix44 mat, vec3 color)
{
	buildAABB(m_immediateLine.get(), aabb, mat, color);
}

void DebugSystem::drawBox(vec3 center, vec3 halfExtents, Matrix44 mat, vec3 color)
{
	buildBox(m_immediateLine.get(), center, halfExtents, mat, color);
}

void DebugSystem::drawSphere(vec3 center, float radius, vec3 color, int segmentCount)
{
	buildSphere(m_immediateLine.get(), center, radius, color, segmentCount);
}

void DebugSystem::drawCylinder(vec3 center, float radius, float height, vec3 color, int segmentCount)
{
	buildCylinder(m_immediateLine.get(), center, radius, height, color, segmentCount);
}

void DebugSystem::drawCapsule(vec3 center, float radius, float height, vec3 color, int segmentCount)
{
	buildCapsule(m_immediateLine.get(), center, radius, height, color, segmentCount);
}

void DebugSystem::drawCone(vec3 center, float radius, float height, vec3 color, int segmentCount)
{
	buildCone(m_immediateLine.get(), center, radius, height, color, segmentCount);
}

void DebugSystem::drawPlane(vec3 center, vec3 normal, float size, vec3 color)
{
	buildPlane(m_immediateLine.get(), center, normal, size, color);
}

void DebugSystem::drawGrid(vec3 center, float width, float depth, int xSegments, int zSegments, vec3 color)
{
	buildGrid(m_immediateLine.get(), center, width, depth, xSegments, zSegments, color);
}

void DebugSystem::drawFrustum(const vec3 corners[8], vec3 color)
{
	buildFrustum(m_immediateLine.get(), corners, color);
}

DebugPrimitive* DebugSystem::createLine(vec3 A, vec3 B, vec3 color)
{
	DebugPrimitive* primitive = createRetainedPrimitive(color);
	buildLine(primitive->getLinePrimitive(), A, B, color);
	return primitive;
}

DebugPrimitive* DebugSystem::createRay(vec3 origin, vec3 dir, float length, vec3 color)
{
	DebugPrimitive* primitive = createRetainedPrimitive(color);
	buildRay(primitive->getLinePrimitive(), origin, dir, length, color);
	return primitive;
}

DebugPrimitive* DebugSystem::createPointCross(vec3 pointInWorld, vec3 color, float size)
{
	DebugPrimitive* primitive = createRetainedPrimitive(color);
	buildPointCross(primitive->getLinePrimitive(), pointInWorld, color, size);
	return primitive;
}

DebugPrimitive* DebugSystem::createAABB(AABB aabb, vec3 color)
{
	Matrix44 identity;
	DebugPrimitive* primitive = createRetainedPrimitive(color);
	buildAABB(primitive->getLinePrimitive(), aabb, identity, color);
	return primitive;
}

DebugPrimitive* DebugSystem::createAABB(AABB aabb, Matrix44 mat, vec3 color)
{
	DebugPrimitive* primitive = createRetainedPrimitive(color);
	buildAABB(primitive->getLinePrimitive(), aabb, mat, color);
	return primitive;
}

DebugPrimitive* DebugSystem::createBox(vec3 center, vec3 halfExtents, Matrix44 mat, vec3 color)
{
	DebugPrimitive* primitive = createRetainedPrimitive(color);
	buildBox(primitive->getLinePrimitive(), center, halfExtents, mat, color);
	return primitive;
}

DebugPrimitive* DebugSystem::createSphere(vec3 center, float radius, vec3 color, int segmentCount)
{
	DebugPrimitive* primitive = createRetainedPrimitive(color);
	buildSphere(primitive->getLinePrimitive(), center, radius, color, segmentCount);
	return primitive;
}

DebugPrimitive* DebugSystem::createCylinder(vec3 center, float radius, float height, vec3 color, int segmentCount)
{
	DebugPrimitive* primitive = createRetainedPrimitive(color);
	buildCylinder(primitive->getLinePrimitive(), center, radius, height, color, segmentCount);
	return primitive;
}

DebugPrimitive* DebugSystem::createCapsule(vec3 center, float radius, float height, vec3 color, int segmentCount)
{
	DebugPrimitive* primitive = createRetainedPrimitive(color);
	buildCapsule(primitive->getLinePrimitive(), center, radius, height, color, segmentCount);
	return primitive;
}

DebugPrimitive* DebugSystem::createCone(vec3 center, float radius, float height, vec3 color, int segmentCount)
{
	DebugPrimitive* primitive = createRetainedPrimitive(color);
	buildCone(primitive->getLinePrimitive(), center, radius, height, color, segmentCount);
	return primitive;
}

DebugPrimitive* DebugSystem::createPlane(vec3 center, vec3 normal, float size, vec3 color)
{
	DebugPrimitive* primitive = createRetainedPrimitive(color);
	buildPlane(primitive->getLinePrimitive(), center, normal, size, color);
	return primitive;
}

DebugPrimitive* DebugSystem::createGrid(vec3 center, float width, float depth, int xSegments, int zSegments, vec3 color)
{
	DebugPrimitive* primitive = createRetainedPrimitive(color);
	buildGrid(primitive->getLinePrimitive(), center, width, depth, xSegments, zSegments, color);
	return primitive;
}

DebugPrimitive* DebugSystem::createFrustum(const vec3 corners[8], vec3 color)
{
	DebugPrimitive* primitive = createRetainedPrimitive(color);
	buildFrustum(primitive->getLinePrimitive(), corners, color);
	return primitive;
}

DebugPrimitive* DebugSystem::createRetainedPrimitive(vec3 defaultColor)
{
	std::unique_ptr<DebugPrimitive> primitive(new DebugPrimitive());
	primitive->setDefaultColor(defaultColor);
	DebugPrimitive* result = primitive.get();
	m_retainedPrimitives.push_back(std::move(primitive));
	return result;
}

void DebugSystem::removeDestroyRequestedPrimitives()
{
	m_retainedPrimitives.erase(std::remove_if(m_retainedPrimitives.begin(), m_retainedPrimitives.end(),
		[](const std::unique_ptr<DebugPrimitive>& primitive)
		{
			return primitive->isDestroyRequested();
		}), m_retainedPrimitives.end());
}

Material* DebugSystem::getDebugWireframeMaterial()
{
	if(!m_debugWireframeMaterial)
	{
		m_debugWireframeMaterial = Material::createFromTemplate("Color");
		m_debugWireframeMaterial->setRenderStage(RenderFlag::RenderStage::DEBUG_LAYER);
		m_debugWireframeMaterial->setPrimitiveTopology(PrimitiveTopology::TriangleList);
		m_debugWireframeMaterial->setRasterFillMode(RasterFillMode::Wireframe);
		m_debugWireframeMaterial->setIsDepthTestEnable(m_wireframeOverlayDepthTestEnabled);
		m_debugWireframeMaterial->setIsDepthWriteEnable(false);
		m_debugWireframeMaterial->setVar("TU_color", vec4(m_wireframeOverlayColor, 1.0f));
	}
	return m_debugWireframeMaterial;
}
}
