#include "ScenePickingSystem.h"

#include "Base/Camera.h"
#include "Engine/Engine.h"
#include "Scene/Scene.h"
#include "Scene/SceneMgr.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace tzw {

namespace
{
constexpr float PickEpsilon = 0.00001f;
constexpr float DefaultCursorRayDistance = 10000.0f;

bool slabRange(float origin, float direction, float minValue, float maxValue, float& tMin, float& tMax)
{
	if (std::fabs(direction) <= PickEpsilon)
	{
		return origin >= minValue && origin <= maxValue;
	}

	float t0 = (minValue - origin) / direction;
	float t1 = (maxValue - origin) / direction;
	if (t0 > t1)
	{
		std::swap(t0, t1);
	}
	tMin = std::max(tMin, t0);
	tMax = std::min(tMax, t1);
	return tMin <= tMax;
}
}

BoxPickShape::BoxPickShape(const AABB& localBounds, const Matrix44& worldTransform)
	: m_localBounds(localBounds),
	  m_worldTransform(worldTransform)
{
}

bool BoxPickShape::hit(const Ray& worldRay, PickHit& outHit) const
{
	bool invertible = false;
	Matrix44 worldTransform = m_worldTransform;
	Matrix44 inverseTransform = worldTransform.inverted(&invertible);
	if (!invertible)
	{
		return false;
	}

	const vec3 localOrigin = (inverseTransform * vec4(worldRay.origin(), 1.0f)).toVec3();
	const vec3 localDirection = (inverseTransform * vec4(worldRay.direction(), 0.0f)).toVec3();
	const vec3 minValue = m_localBounds.min();
	const vec3 maxValue = m_localBounds.max();

	float tMin = 0.0f;
	float tMax = std::numeric_limits<float>::max();
	if (!slabRange(localOrigin.x, localDirection.x, minValue.x, maxValue.x, tMin, tMax)
		|| !slabRange(localOrigin.y, localDirection.y, minValue.y, maxValue.y, tMin, tMax)
		|| !slabRange(localOrigin.z, localDirection.z, minValue.z, maxValue.z, tMin, tMax))
	{
		return false;
	}

	outHit.hit = true;
	outHit.distance = std::max(0.0f, tMin);
	outHit.worldPoint = worldRay.origin() + worldRay.direction() * outHit.distance;
	return true;
}

bool PickHandle::isValid() const
{
	return index >= 0 && generation != 0;
}

PickHandle ScenePickingSystem::registerTarget(PickTargetDesc desc)
{
	PickTarget target;
	target.category = desc.category;
	target.priority = desc.priority;
	target.owner = desc.owner;
	target.payload = desc.payload;
	target.enabled = desc.enabled;
	target.shape = std::move(desc.shape);

	PickHandle handle;
	handle.index = static_cast<int>(m_targets.size());
	handle.generation = target.generation;
	m_targets.push_back(std::move(target));
	return handle;
}

void ScenePickingSystem::unregisterTarget(PickHandle handle)
{
	if (!isLiveHandle(handle))
	{
		return;
	}

	PickTarget& target = m_targets[handle.index];
	target.generation += 1;
	target.category = 0;
	target.priority = 0;
	target.owner = nullptr;
	target.payload = PickPayload();
	target.enabled = false;
	target.shape.reset();
}

void ScenePickingSystem::setTargetEnabled(PickHandle handle, bool enabled)
{
	if (!isLiveHandle(handle))
	{
		return;
	}
	m_targets[handle.index].enabled = enabled;
}

void ScenePickingSystem::updateTargetShape(PickHandle handle, std::unique_ptr<PickShape> shape)
{
	if (!isLiveHandle(handle))
	{
		return;
	}
	m_targets[handle.index].shape = std::move(shape);
}

bool ScenePickingSystem::pick(const Ray& ray, uint32_t categoryMask, PickResult& outResult) const
{
	bool hasResult = false;
	float bestDistance = std::numeric_limits<float>::max();
	int bestPriority = std::numeric_limits<int>::min();

	for (int i = 0; i < static_cast<int>(m_targets.size()); ++i)
	{
		const PickTarget& target = m_targets[i];
		if (!target.enabled || !target.shape || (target.category & categoryMask) == 0)
		{
			continue;
		}

		PickHit hit;
		if (!target.shape->hit(ray, hit))
		{
			continue;
		}

		const bool closer = hit.distance < bestDistance - PickEpsilon;
		const bool sameDistanceHigherPriority = std::fabs(hit.distance - bestDistance) <= PickEpsilon
			&& target.priority > bestPriority;
		if (!hasResult || closer || sameDistanceHigherPriority)
		{
			hasResult = true;
			bestDistance = hit.distance;
			bestPriority = target.priority;
			outResult.handle.index = i;
			outResult.handle.generation = target.generation;
			outResult.category = target.category;
			outResult.priority = target.priority;
			outResult.owner = target.owner;
			outResult.payload = target.payload;
			outResult.distance = hit.distance;
			outResult.worldPoint = hit.worldPoint;
		}
	}

	return hasResult;
}

bool ScenePickingSystem::makeCursorRay(Ray& outRay, float* outMaxDistance)
{
	Scene* scene = g_GetCurrScene();
	Camera* camera = scene ? scene->defaultCamera() : nullptr;
	if (!camera)
	{
		return false;
	}

	const vec2 mousePos = Engine::shared()->getMousePos();
	const vec3 nearPoint = camera->unproject(vec3(mousePos.x, mousePos.y, 0.0f));
	const vec3 farPoint = camera->unproject(vec3(mousePos.x, mousePos.y, 1.0f));
	const vec3 direction = safeNormalized(farPoint - nearPoint, camera->getForward());
	outRay = Ray(camera->getPos(), direction);
	if (outMaxDistance)
	{
		*outMaxDistance = DefaultCursorRayDistance;
	}
	return true;
}

bool ScenePickingSystem::isLiveHandle(PickHandle handle) const
{
	return handle.index >= 0
		&& handle.index < static_cast<int>(m_targets.size())
		&& m_targets[handle.index].generation == handle.generation
		&& m_targets[handle.index].shape;
}

} // namespace tzw
